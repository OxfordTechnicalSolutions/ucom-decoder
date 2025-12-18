message_dict = {
            "MessageSchemaVersion": 2,
            "MessageID": 64512,
            "MessageVersion": 0,
            "MessageName": "Custom Test",
            "MessageDescription": "Custom message for testing some signals",
            "MessageTiming": "SDN",
            "MessageEnabled": False,
            "OutputType": "FrequencyBased",
            "FrequencyOutputType": "IMU",
            "OutputFrequencyDecimation": 1,
            "SignalsInMessage": [
                {
                    "DataType": "U32",
                    "Offset": 0.0,
                    "ScaleFactor": 1,
                    "SignalID": "UCOMVersion",
                    "SourceID": "META",
                    "Unit": "s"
                },
                {
                    "DataType": "STR",
                    "Offset": 0,
                    "ScaleFactor": 1,
                    "SignalID": "DevID",
                    "SourceID": "CONFIG",
                    "Unit": "-"
                },
                {
                    "DataType": "F64",
                    "Offset": 0,
                    "ScaleFactor": 1,
                    "SignalID": "Wd",
                    "SourceID": "BNS_SDN",
                    "Unit": "deg/s"
                }
            ]
        }