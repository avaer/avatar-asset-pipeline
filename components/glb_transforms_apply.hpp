/* avatar-build is distributed under MIT license:
 *
 * Copyright (c) 2021 Kota Iguchi
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#pragma once

#include <DSPatch.h>
#include "pipelines.hpp"
#include <iostream>

namespace DSPatch {

class glb_transforms_apply final : public Component {

public:
    glb_transforms_apply(AvatarBuild::cmd_options* options)
        : Component()
        , options(options)

    {
        SetInputCount_(3);
        SetOutputCount_(3);
    }

    virtual ~glb_transforms_apply()
    {
    }

protected:
    virtual void Process_(SignalBus const& inputs, SignalBus& outputs) override
    {
        // just return immediately when there's critical error in previous component
        const auto discarded = inputs.GetValue<bool>(0);
        if (discarded && *discarded) {
            return;
        }
        AVATAR_PIPELINE_LOG("[INFO] glb_transforms_apply");

        const auto data_ptr = inputs.GetValue<cgltf_data*>(1);
        const auto bones_ptr = inputs.GetValue<AvatarBuild::bone_mappings*>(2);

        if (data_ptr && bones_ptr) {
            cgltf_data* data = *data_ptr;
            AvatarBuild::bone_mappings* mappings = *bones_ptr;
            gltf_apply_transforms(data, mappings->name_to_node);
            gltf_update_inverse_bind_matrices(data);
            gltf_remove_animation(data); // pose change does not work well with animation
            outputs.SetValue(0, false);    // discarded
            outputs.SetValue(1, data);
            outputs.SetValue(2, *bones_ptr);
        } else {
            AVATAR_PIPELINE_LOG("[ERROR] glb_transforms_apply: input.1 not found");
            outputs.SetValue(0, true);    // discarded
        }
    }
    AvatarBuild::cmd_options* options;
};

} // namespace DSPatch