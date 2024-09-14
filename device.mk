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

# FastbootD
PRODUCT_PACKAGES += fastbootd

# GPU Firmware
PRODUCT_COPY_FILES += \
    vendor/samsung/e1s/proprietary/recovery/root/lib/firmware/sgpu/vangogh_lite_unified_evt1.bin:$(TARGET_COPY_OUT_VENDOR_RAMDISK)/lib/firmware/sgpu/vangogh_lite_unified_evt1.bin

# Health
PRODUCT_PACKAGES += \
    android.hardware.health-service.example \
    android.hardware.health-service.example_recovery

# Init
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/fstab.s5e9945:$(TARGET_COPY_OUT_VENDOR_RAMDISK)/fstab.s5e9945

# Partitions
PRODUCT_USE_DYNAMIC_PARTITIONS := true

# Toolbox
PRODUCT_PACKAGES += \
    linker.vendor_ramdisk \
    toolbox.vendor_ramdisk

# Proprietary files
$(call inherit-product, vendor/samsung/e1s/e1s-vendor.mk)
