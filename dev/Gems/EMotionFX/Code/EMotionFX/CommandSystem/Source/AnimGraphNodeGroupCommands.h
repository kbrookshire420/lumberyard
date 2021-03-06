/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/

#pragma once

// include the required headers
#include "CommandSystemConfig.h"
#include <MCore/Source/Command.h>
#include <MCore/Source/CommandGroup.h>
#include <EMotionFX/Source/AnimGraphNodeGroup.h>
#include <EMotionFX/Source/AnimGraph.h>

namespace CommandSystem
{
    // adjust a node group
    MCORE_DEFINECOMMAND_START(CommandAnimGraphAdjustNodeGroup, "Adjust anim graph node group", true)
public:
    static MCore::String GenerateNodeNameString(EMotionFX::AnimGraph* animGraph, const MCore::Array<uint32>& nodeIDs);

    MCore::String               mOldName;
    bool                        mOldIsVisible;
    MCore::RGBAColor            mOldColor;
    MCore::Array<uint32>        mOldNodeIDs;
    bool                        mOldDirtyFlag;
    MCORE_DEFINECOMMAND_END


    // add node group
        MCORE_DEFINECOMMAND_START(CommandAnimGraphAddNodeGroup, "Add anim graph node group", true)
    bool                mOldDirtyFlag;
    MCore::String       mOldName;
    MCORE_DEFINECOMMAND_END


    // remove a node group
        MCORE_DEFINECOMMAND_START(CommandAnimGraphRemoveNodeGroup, "Remove anim graph node group", true)
    MCore::String           mOldName;
    bool                    mOldIsVisible;
    MCore::RGBAColor        mOldColor;
    MCore::Array<uint32>    mOldNodeIDs;
    bool                    mOldDirtyFlag;
    MCORE_DEFINECOMMAND_END


    // helper function
    COMMANDSYSTEM_API void ClearNodeGroups(EMotionFX::AnimGraph* animGraph, MCore::CommandGroup* commandGroup = nullptr);
} // namespace CommandSystem
