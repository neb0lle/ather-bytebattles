#!/bin/bash

# Apply the patch
git apply --reject --whitespace=fix patch

# Remove the specified directories
# rm -rf asdk-gen2/platform/cyt2b75/sdk/common/src/drivers/cxpi
# rm -rf asdk-gen2/platform/cyt2b75/sdk/common/src/drivers/smif
# rm -rf asdk-gen2/platform/cyt2b75/sdk/tviibe1m/hdr/rev_b
# rm -rf asdk-gen2/platform/cyt2b75/sdk/tviibe1m/hdr/rev_c

echo "Patch applied."