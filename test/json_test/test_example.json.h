#pragma once

const char* json_data = R"!!({
  "key_with_string": "value",
  "key_with_int": 1,
  "key_with_object": {
    "key_in_object_with_string": "the string",
    "key_in_object_with_double": 1.2345,
    "key_in_object_with_false": false,
    "key_in_object_with_true": true
  },
  "key_with_empty_object": {},
  "key_with_true": true,
  "key_with_false": false,
  "io": {
    "naming": {
      "analog": {
        "a0": "IR livingroom",
        "a1": "IR hallway"
      },
      "digital": {
        "input": {
          "i0": "Front door",
          "i1": "Side door"
        },
        "output": {
          "o0": "Siren"
        }
      }
    },
    "reference_values": {
      "analog": {
        "a0": {
          "ref": 1234,
          "variance": 200
        },
        "a1": {
          "ref": 5678,
          "variance": 250
        }
      },
      "digital": {
        "i0": {
          "ref": true
        },
        "i1": {
          "ref": false
        }
      }
    },
    "startup_state": {
      "digital": {
        "o0": false,
        "o1": true
      }
    }
  },
  "array": [
    "a0",
    "a1",
    "a2",
    "a3",
    "a4",
    {
      "value": true,
      "value2": false
    }
  ],
  "zones": {
    "night": [
      "a0",
      "a1"
    ],
    "all": [
      "a0",
      "a1",
      "a2",
      "a3",
      "a4"
    ]
  }
})!!";