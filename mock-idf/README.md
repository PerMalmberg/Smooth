# mock-idf

Mock-idf is a minimal mock of Espressif's ESP-IDF framework that allows Smooth-based applications to build on Linux
without while still referencing IDF headers and functions etc. Only the bare minimum is mocked.

Note: This is a **mock**, not a simulator, so don't expect things like I2C to actually give you back any data.

# License notice regarding header files borrowed from ESP-IDF

Some header files in mock-idf have their origin in whole or in part in [ESP-IDF](https://github.com/espressif/esp-idf).
As such those parts have the following license applied to them:

    Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.    