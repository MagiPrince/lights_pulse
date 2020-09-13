set(srcs 
        "main.c"
        "audio_module/bluetooth/bt_audio_core.c"
        "audio_module/bluetooth/bt_a2dp.c"
        "audio_module/bluetooth/bt_avrcp.c"
        "dmx_module/dmx.c"
        "dmx_module/MF078.c"
        "dmx_module/MPX405.c"
        "dmx_module/dmx_task.c"
        "beat_detection_module/beat_detector.c"
        "beat_detection_module/tdf.c"
        "beat_detection_module/beat_detector_task.c")

set(include_dirs
        "."
        "audio_module"
        "audio_module/bluetooth"
        "dmx_module"
        "beat_detection_module")

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "${include_dirs}")
