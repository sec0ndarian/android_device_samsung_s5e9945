#!/usr/bin/env -S PYTHONPATH=../../../tools/extract-utils python3
#
# SPDX-FileCopyrightText: 2024 The LineageOS Project
# SPDX-License-Identifier: Apache-2.0
#

from extract_utils.fixups_blob import (
    blob_fixup,
    blob_fixups_user_type,
)
from extract_utils.main import (
    ExtractUtils,
    ExtractUtilsModule,
)

namespace_imports = [
    'device/samsung/e1s',
]


blob_fixups: blob_fixups_user_type = {
    'vendor/etc/init/init.s5e9945.rc': blob_fixup()
        .regex_replace('vendor_spay', 'system'),
}  # fmt: skip

module = ExtractUtilsModule(
    'e1s',
    'samsung',
    blob_fixups=blob_fixups,
    namespace_imports=namespace_imports,
)

if __name__ == '__main__':
    utils = ExtractUtils.device(module)
    utils.run()
