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
#ifndef UNDO_SYSTEM_H
#define UNDO_SYSTEM_H

#include <AzCore/base.h>
#include <AzCore/Memory/SystemAllocator.h>
#include <AzCore/RTTI/RTTI.h>
#include <AzCore/EBus/EBus.h>
#include <AzCore/std/containers/ring_buffer.h>
#include <AzCore/std/string/string.h>

#pragma once

namespace AzToolsFramework
{
    namespace UndoSystem
    {
        typedef AZ::u64 URCommandID;

        // plug one of these into your undo stack if you'd like notification.
        class IUndoNotify
        {
        public:
            virtual ~IUndoNotify() = default;
            virtual void OnUndoStackChanged() = 0;
        };

        class URSequencePoint
        {
        public:
            AZ_RTTI(URSequencePoint, "{D6A52DA5-DF44-43BE-B42C-B6E88BDF476A}")
            AZ_CLASS_ALLOCATOR(URSequencePoint, AZ::SystemAllocator, 0)

            typedef AZStd::vector<URSequencePoint*> ChildVec;

            /**
            Usage: construct a standalone command which can both implement undo+redo and have children commands
            */
            explicit URSequencePoint(const AZStd::string& friendlyName, URCommandID id = 0);

            /**
            Usage: construct a child command which can both implement undo+redo and have children commands
            plus automatically create a friendly child name and append to the parent's children
            NB: Parent takes ownership of the allocation
            */
            explicit URSequencePoint(URCommandID id);

            /**
            Usage: base implementation automatically deletes all children recursively
            therefore only a topmost parent needs to be deleted from outside
            */
            virtual ~URSequencePoint();

            void RunUndo();
            void RunRedo();

            /**
            Usage: override with class specific actions
            */
            virtual void Undo();
            virtual void Redo();

            /**
            Usage:  override with class specific change comparison between undo/redo state
            default to false
            */
            virtual bool Changed() const { return false; }

            /**
            Usage: return the first command in the parent/child tree with a matching id
            returns NULL on failure to make any match
            */
            URSequencePoint* Find(URCommandID id, const AZ::Uuid& typeOfCommand);

            void SetName(const AZStd::string& friendlyName);
            AZStd::string& GetName();
            
            void SetParent(URSequencePoint* parent);
            URSequencePoint* GetParent() const { return m_parent; }
            
            const ChildVec& GetChildren() const { return m_children; }
            bool HasRealChildren() const;

            /**
            Usage: pass a function callback that eats a URSequencePoint*
            this walks the child tree and applies the callback to each URSequencePoint
            // example
            // commandPtr->ApplyToTree( AZStd::bind(&MyClass::DoSomethingWithCommandPtr, this, AZStd::placeholders::_1) );
            */
            typedef AZStd::function< void(URSequencePoint*) > ApplyOperationCB;
            void ApplyToTree(const ApplyOperationCB& applyCB);

            bool IsPosted() const { return m_isPosted; }

            // default id 0 is never equal, commands are only equal if IDs otherwise match
            bool operator==(const URCommandID id) const { return m_id == 0 ? false : m_id == id; }
            bool operator==(const URSequencePoint* com) const { return *this == com->m_id; }

            friend class UndoStack;

        protected:
            void AddChild(URSequencePoint*);
            void RemoveChild(URSequencePoint*);
            AZStd::string m_friendlyName;
            URCommandID m_id;

            ChildVec m_children;
            URSequencePoint* m_parent;
            bool m_isPosted;
        };

        //--------------------------------------------------------------------

        class UndoStack
        {
        public:

            AZ_CLASS_ALLOCATOR(UndoStack, AZ::SystemAllocator, 0);

            UndoStack(int limit, IUndoNotify* notify);
            ~UndoStack();

            URSequencePoint* Post(URSequencePoint*);
            URSequencePoint* Undo();
            URSequencePoint* Redo();

            /**
            Usage: return the first command in all active command trees with a matching id
            returns NULL on failure to make any match
            */
            URSequencePoint* Find(URCommandID id, const AZ::Uuid& typeOfCommand);

            // Moves the clean state to the current point.
            // does not slice, you can undo back past the clean point.
            void SetClean();
            bool IsClean() const;

            void Reset(); // clear everything

            bool CanUndo() const;
            bool CanRedo() const;

            const char* GetRedoName() const;
            const char* GetUndoName() const;

            // get the sequence point that would be undone right now if you hit undo.
            URSequencePoint* GetTop();

            URSequencePoint* PopTop(); // by doing this, you take ownership of the memory.

            /**
            Usage: slices off all commands above the current cursor
            example: undo followed by new commands should slice to remove redo commands no longer viable
            */
            void Slice();

        protected:
#ifdef _DEBUG
            void CleanCheck();
#endif

            int m_Cursor;
            int m_CleanPoint;

            typedef AZStd::ring_buffer<URSequencePoint*> SequencePointBuffer;

            SequencePointBuffer m_SequencePointsBuffer;
            IUndoNotify* m_notify;

        private:

            // undo operations are not reentrant
            volatile bool reentryGuard;
        };
    }
} // namespace AzToolsFramework

#endif
