#
# Copyright 2014 The Android Open-Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# API Levels
PRODUCT_SHIPPING_API_LEVEL := 34

# Dynamic Partitions
PRODUCT_USE_DYNAMIC_PARTITIONS := true

# Soong Namespaces
PRODUCT_SOONG_NAMESPACES := hardware/samsung


# Display
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.opengles.aep.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.opengles.aep.xml \
    frameworks/native/data/etc/android.hardware.vulkan.compute-0.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.vulkan.compute-0.xml \
    frameworks/native/data/etc/android.hardware.vulkan.level-1.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.vulkan.level-1.xml \
    frameworks/native/data/etc/android.hardware.vulkan.version-1_3.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.vulkan.version-1_3.xml \
    frameworks/native/data/etc/android.software.opengles.deqp.level-latest.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.opengles.deqp.level-latest.xml \
    frameworks/native/data/etc/android.software.vulkan.deqp.level-2023-03-01.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.vulkan.deqp.level-2023-03-01.xml \
    vendor/samsung/e1s/proprietary/recovery/root/lib/firmware/sgpu/vangogh_lite_unified_evt1.bin:$(TARGET_COPY_OUT_VENDOR_RAMDISK)/lib/firmware/sgpu/vangogh_lite_unified_evt1.bin

# Fastbootd
PRODUCT_PACKAGES += fastbootd

# Health
PRODUCT_PACKAGES += \
    android.hardware.health-service.example \
    android.hardware.health-service.example_recovery

# Init
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/init/fstab.s5e9945:$(TARGET_COPY_OUT_VENDOR)/etc/fstab.s5e9945 \
    $(LOCAL_PATH)/configs/init/fstab.s5e9945:$(TARGET_COPY_OUT_VENDOR_RAMDISK)/fstab.s5e9945

# Kernel Modules
PRODUCT_COPY_FILES += /dev/null:$(TARGET_COPY_OUT_RECOVERY)/root/dev/null

PRODUCT_PACKAGES += \
    linker.vendor_ramdisk \
    toolbox.vendor_ramdisk

# Keystore
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.keystore.app_attest_key.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.keystore.app_attest_key.xml

# USB
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/init/init.s5e9945.usb.rc:$(TARGET_COPY_OUT_VENDOR)/etc/init/init.s5e9945.usb.rc \
    frameworks/native/data/etc/android.hardware.usb.accessory.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.usb.accessory.xml \
    frameworks/native/data/etc/android.hardware.usb.host.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.usb.host.xml

PRODUCT_PACKAGES += \
    android.hardware.usb-service.samsung \
    android.hardware.usb.gadget-service.samsung

# VNDK
PRODUCT_PACKAGES += vndservicemanager


# Dalvik Heap
$(call inherit-product, frameworks/native/build/phone-xhdpi-6144-dalvik-heap.mk)

# Proprietary Files
$(call inherit-product, vendor/samsung/e1s/e1s-vendor.mk)
