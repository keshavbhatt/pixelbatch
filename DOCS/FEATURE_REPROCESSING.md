# Re-processing Feature Implementation

## Overview
Added support for re-processing completed or failed image optimization tasks, allowing users to test different optimizer settings without removing and re-adding images.

## Problem Statement

**Before:**
- Once images were processed (status: Completed), the "Process Images" button remained disabled
- Users had to remove images and re-add them to try different settings
- No way to re-optimize with adjusted quality/compression settings
- Failed tasks couldn't be easily retried

**User Pain Points:**
- ❌ Can't experiment with different settings
- ❌ Tedious workflow: remove → re-add → process
- ❌ Lost efficiency when testing optimal settings
- ❌ No retry mechanism for errors

## Solution Implemented

### 1. **Enhanced processImages() Method**

Modified `TaskWidget::processImages()` to handle re-processing:

```cpp
void TaskWidget::processImages() {
  int queuedCount = 0;
  
  for (ImageTask *imageTask : qAsConst(m_imageTasks)) {
    // Process pending tasks (new images)
    if (imageTask->taskStatus == ImageTask::Pending) {
      imageTask->taskStatus = ImageTask::Queued;
      m_imageTaskQueue.enqueue(imageTask);
      updateTaskStatus(imageTask);
      queuedCount++;
    }
    // Re-process completed or error tasks
    else if (imageTask->taskStatus == ImageTask::Completed || 
             imageTask->taskStatus == ImageTask::Error) {
      // Reset to Queued
      imageTask->taskStatus = ImageTask::Queued;
      m_imageTaskQueue.enqueue(imageTask);
      updateTaskStatus(imageTask);
      
      // Clear previous results
      updateTaskSizeAfter(imageTask, "—");
      updateTaskSaving(imageTask, "—");
      
      queuedCount++;
    }
  }
  
  // ...rest of processing
}
```

**Key Changes:**
- Added support for `Completed` and `Error` task statuses
- Resets task status to `Queued`
- Clears previous optimization results (size after, savings)
- Re-queues tasks for processing

### 2. **Smart Button State Management**

Updated `updateStatusBarButtons()` in PixelBatch:

```cpp
void PixelBatch::updateStatusBarButtons(bool processing) {
  if (m_taskWidget) {
    auto taskStatusCounts = m_taskWidget->getTaskStatusCounts();
    
    // Enable if there are pending OR completed/error tasks
    bool hasTasksToProcess = taskStatusCounts.pendingCount > 0 || 
                             taskStatusCounts.completedCount > 0 || 
                             taskStatusCounts.errorCount > 0;
    
    m_statusBarProcessButton->setEnabled(hasTasksToProcess && !processing);
    
    // Dynamic button text
    if (taskStatusCounts.pendingCount > 0) {
      m_statusBarProcessButton->setText(tr("Process Images"));
    } else if (taskStatusCounts.completedCount > 0 || taskStatusCounts.errorCount > 0) {
      m_statusBarProcessButton->setText(tr("Re-process Images"));
    }
    
    m_statusBarAddButton->setEnabled(!processing);
  }
}
```

**Key Changes:**
- Button enabled for completed/error tasks
- Dynamic button text: "Process Images" → "Re-process Images"
- Clear visual indication of re-processing mode

### 3. **Consistent Menu Actions**

Updated `updateMenuActions()` with same logic for consistency.

## Features

### ✅ **Re-process Completed Images**
- After successful optimization, button changes to "Re-process Images"
- Click to re-optimize with new settings
- Previous results cleared automatically

### ✅ **Retry Failed Images**
- Error tasks can be re-processed
- Useful after fixing configuration issues
- Clear error state before retry

### ✅ **Mixed State Support**
- Works with mix of pending, completed, and error tasks
- Processes all eligible tasks in one click
- Maintains separate task statuses

### ✅ **Visual Feedback**
- Button text changes based on task states
- Status messages show queue count
- Color-coded status indicators preserved

## Use Cases

### 1. **Testing Different Quality Settings**

**Workflow:**
1. User adds 10 JPEGs
2. Processes with quality=90
3. Reviews results (30% savings)
4. Changes quality=80 in settings
5. Clicks "Re-process Images"
6. Compares new results (45% savings)

### 2. **Format-Specific Optimization**

**Workflow:**
1. User adds PNGs
2. Processes with default settings
3. Sees moderate compression
4. Enables color reduction in PNG settings
5. Re-processes with new settings
6. Gets better compression

### 3. **Error Recovery**

**Workflow:**
1. User processes images
2. Some fail due to missing tool (svgo not installed)
3. User installs svgo
4. Clicks "Re-process Images"
5. Failed tasks retry automatically

### 4. **A/B Testing Settings**

**Workflow:**
1. Process with lossless settings
2. Note file sizes
3. Change to lossy settings
4. Re-process
5. Compare results
6. Choose optimal balance

## Benefits

### For Users
- ✅ **Easy experimentation**: Test different settings quickly
- ✅ **No tedious re-adding**: Keep images in list
- ✅ **Error recovery**: Retry failed tasks easily
- ✅ **Visual clarity**: Button text shows mode
- ✅ **Flexible workflow**: Supports iterative optimization

### For Developers
- ✅ **Clean implementation**: Minimal code changes
- ✅ **Backward compatible**: Existing flow unchanged
- ✅ **Consistent patterns**: Same logic in multiple places
- ✅ **Well integrated**: Works with existing status system

## Technical Details

### Task State Transitions

**Before:**
```
Pending → Queued → Processing → Completed/Error (final)
```

**After:**
```
Pending → Queued → Processing → Completed/Error
                                      ↓
                                   Queued (re-process)
                                      ↓
                                 Processing → Completed/Error
```

### Button State Logic

| Pending | Completed | Error | Button State | Button Text |
|---------|-----------|-------|--------------|-------------|
| > 0 | Any | Any | Enabled | "Process Images" |
| 0 | > 0 | Any | Enabled | "Re-process Images" |
| 0 | Any | > 0 | Enabled | "Re-process Images" |
| 0 | 0 | 0 | Disabled | "Process Images" |

### Data Clearing

When re-processing, these fields are reset:
- Status: Set to `Queued`
- Size After: Reset to "—"
- Savings: Reset to "—"
- Color coding: Reset to queue color (blue)

Original data preserved:
- File path
- Original file size
- Task reference

## Testing Recommendations

### Test Cases

1. **Basic Re-processing**
   - [ ] Add image, process, verify "Re-process" button appears
   - [ ] Click re-process, verify task re-runs
   - [ ] Check previous results cleared

2. **Settings Changes**
   - [ ] Process with quality=90
   - [ ] Change to quality=70
   - [ ] Re-process and verify different results

3. **Error Recovery**
   - [ ] Cause processing error (remove optimizer tool)
   - [ ] Fix error
   - [ ] Re-process and verify success

4. **Mixed States**
   - [ ] Mix of pending, completed, error tasks
   - [ ] Verify all get processed
   - [ ] Check status updates correctly

5. **Button States**
   - [ ] Verify button text changes
   - [ ] Check enable/disable logic
   - [ ] Test during processing (should disable)

### Edge Cases
- [ ] Empty task list
- [ ] All tasks queued
- [ ] Re-process during processing (should be disabled)
- [ ] Rapid clicking re-process button

## Documentation Updates

### README.md
- Added re-processing to "Basic Usage" workflow
- Added tips for testing different settings
- Explained use case for re-optimization

### Files Modified
1. `src/taskwidget.cpp` - Enhanced processImages()
2. `src/pixelbatch.cpp` - Updated button logic (2 methods)
3. `README.md` - User documentation

## Future Enhancements

Potential improvements:
- [ ] "Re-process Selected" for individual tasks
- [ ] Settings comparison view (before/after)
- [ ] Preset switching with auto-reprocess
- [ ] Undo last re-process
- [ ] Batch re-process history

## Conclusion

The re-processing feature significantly improves UX by:
- **Eliminating tedious workflows** (no more remove/re-add)
- **Enabling experimentation** (test different settings easily)
- **Supporting error recovery** (retry failed tasks)
- **Providing visual clarity** (button text changes)

This is a **natural and expected feature** that users will appreciate, especially when fine-tuning optimizer settings for their specific needs.

---

**Author:** PixelBatch Development Team  
**Date:** November 6, 2025  
**Status:** ✅ Complete

