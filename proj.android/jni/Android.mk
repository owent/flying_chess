LOCAL_PATH := $(call my-dir)
# LOCAL_ABS_PATH := $(LOCAL_PATH)
LOCAL_ABS_PATH := $(LOCAL_PATH)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/cocos)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/cocos/editor-support/cocostudio)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/extensions)

LOCAL_MODULE := flying_chess_shared

LOCAL_MODULE_FILENAME := libflying_chess
   
FLYING_SRC_FILES += $(wildcard $(LOCAL_PATH)/../../Classes/*.cpp)  
LOCAL_SRC_FILES := $(FLYING_SRC_FILES:$(LOCAL_PATH)/%=%) 
LOCAL_SRC_FILES += main.cpp

LOCAL_C_INCLUDES := $(LOCAL_ABS_PATH)/../../Classes \
					$(LOCAL_ABS_PATH)/../../cocos2d \
					$(LOCAL_ABS_PATH)/../../cocos2d/cocos \
					$(LOCAL_ABS_PATH)/../../cocos2d/cocos/editor-support \
					$(LOCAL_ABS_PATH)/../../cocos2d/cocos/editor-support/cocostudio

# $(warning  $(LOCAL_C_INCLUDES) )

LOCAL_WHOLE_STATIC_LIBRARIES := cocos2dx_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocosdenshion_static

# LOCAL_WHOLE_STATIC_LIBRARIES += box2d_static
# LOCAL_WHOLE_STATIC_LIBRARIES += cocosbuilder_static
# LOCAL_WHOLE_STATIC_LIBRARIES += spine_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocostudio_static
# LOCAL_WHOLE_STATIC_LIBRARIES += cocos_network_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos_extension_static


include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)
$(call import-module,audio/android)

# $(call import-module,Box2D)
# $(call import-module,editor-support/cocosbuilder)
# $(call import-module,editor-support/spine)
$(call import-module,editor-support/cocostudio)
# $(call import-module,network)
$(call import-module,extensions)
