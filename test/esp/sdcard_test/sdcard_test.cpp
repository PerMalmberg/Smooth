#include "sdcard_test.h"
#include <fstream>
#include <algorithm>
#include <random>
#include <cstring>
#include <cstdio>
#include <smooth/core/filesystem/MMCSDCard.h>
#include <smooth/core/filesystem/SPISDCard.h>
#include <smooth/core/timer/ElapsedTime.h>
#include <smooth/core/logging/log.h>
#include <smooth/core/task_priorities.h>

using namespace smooth;
using namespace smooth::core;
using namespace smooth::core::ipc;
using namespace smooth::core::logging;
using namespace smooth::core::timer;
using namespace std::chrono;

// Select appropriate interface - SPI or MMC
//#define USE_SPI

// Set to true to use 1-line MMC mode.
const bool MMC_1_LINE_MODE = false;


namespace sdcard_test
{
    static const char *tag = "sdcard_test";

    App::App()
            : Application(APPLICATION_BASE_PRIO,
                          std::chrono::milliseconds(1000))
    {
    }

    void App::init()
    {
        Application::init();

#ifdef USE_SPI
        // Change the pin configuration to match your hardware.
        card = std::make_unique<smooth::core::filesystem::SPISDCard>(GPIO_NUM_19,
                                                                     GPIO_NUM_23,
                                                                     GPIO_NUM_18,
                                                                     GPIO_NUM_5,
                                                                     GPIO_NUM_21);
#else
        card = std::make_unique<smooth::core::filesystem::MMCSDCard>(GPIO_NUM_15,
                                                                     GPIO_NUM_2,
                                                                     GPIO_NUM_26,
                                                                     GPIO_NUM_12,
                                                                     GPIO_NUM_13,
                                                                     MMC_1_LINE_MODE);
#endif

    }

    static bool test_done = false;

    void App::tick()
    {
        ElapsedTime wait;
        wait.start();
        while (wait.get_running_time() < seconds{5})
        {
            Log::info(tag, "If you have JTAG connected to the ESP, it is suggested you"
                           " disconnect them now as they interfere with the SD Card lines in MMC mode");
            std::this_thread::sleep_for(seconds{1});
        }

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, 10000);

        if (!test_done && !card->is_initialized())
        {
            if (card->init("/sdcard", false, 5))
            {
                Log::info(tag, "Starting performance test");

                const size_t total_size = 10 * 1024 * 1024;
                bool error = false;

                // Note: Too large chunk_size and you'll run into out of memory issues. I've seen them at 16k.
                for (size_t chunk_size = 1024 / sizeof(int); !error && chunk_size <= 1024 / sizeof(int) * 12; chunk_size += 1024)
                {
                    std::string path = "/sdcard/";
                    path.append(std::to_string(chunk_size));
                    Log::info(tag, Format("Writing to {1}", Str(path)));

                    ElapsedTime t;
                    t.reset();

                    Log::info(tag,
                              Format("Writing {1} bytes, chunk size {2}.", UInt64{total_size}, UInt64{chunk_size}));
                    auto write = std::make_unique<int[]>(chunk_size);
                    auto read = std::make_unique<int[]>(chunk_size);
                    auto block_size = sizeof(int) * chunk_size;

                    auto fp = fopen(path.c_str(), "wb+");
                    if (fp)
                    {
                        int write_count = 0;
                        size_t written_bytes = 0;
                        for (int i = 0; !error && i <= total_size; i += chunk_size)
                        {
                            // Fill block with random data
                            for (int j = 0; j < chunk_size; ++j)
                            {
                                write[j] = dis(gen);
                            }

                            fseek(fp, 0, SEEK_END);
                            auto written_result = fwrite(write.get(), sizeof(int), chunk_size, fp);
                            if (written_result != chunk_size)
                            {
                                Log::error(tag, "Failed to write");
                                error = true;
                            }
                            else
                            {
                                // Read from file
                                // Must seek when a read follows a write and vise-versa.
                                if (0 != fseek(fp, written_bytes, SEEK_SET))
                                {
                                    Log::error(tag, "Failed to seek");
                                    error = true;
                                }

                                written_bytes += block_size;

                                auto read_res = fread(read.get(), sizeof(int), chunk_size, fp);
                                if (read_res != chunk_size)
                                {
                                    Log::error(tag, "Failed to read");
                                    error = true;
                                }
                            }

                            // Verify read block
                            auto equal = std::memcmp(write.get(), read.get(), block_size);

                            if (equal == 0)
                            {
                                if (++write_count % 1000 == 0)
                                {
                                    Log::info(tag, Format("Still working {1}", Int32(i)));
                                }
                            }
                            else
                            {
                                Log::error(tag, Format("Data not equal, chunk size {1}", Int64(chunk_size)));
                                std::this_thread::sleep_for(seconds{1});
                                error = true;
                            }
                        }

                        t.stop();

                        std::string speed;

                        auto second = duration_cast<std::chrono::seconds>(t.get_running_time());
                        if (second.count() < 1)
                        {
                            speed = Format("{1} bytes/ms.\n", Int64(total_size / duration_cast<std::chrono::milliseconds>(t.get_running_time()).count()));
                            Log::info(tag, speed.c_str());
                        }
                        else
                        {
                            speed = Format("{1} bytes/s, ", Int64(total_size / std::max<int64_t>(1, second.count())));
                            Log::info(tag, speed.c_str());
                        }

                        if (!error)
                        {
                            std::string eof{"EOF"};
                            fwrite(speed.c_str(), sizeof(char), speed.length(), fp);
                            fwrite(eof.c_str(), sizeof(char), eof.length(), fp);
                        }

                        fclose(fp);
                    }
                    else
                    {
                        Log::error(tag, "Could not open file");
                    }
                }

                card->deinit();
                test_done = true;
            }
        }
    }
}