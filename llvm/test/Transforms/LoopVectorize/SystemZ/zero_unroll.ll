; NOTE: Assertions have been autogenerated by utils/update_test_checks.py UTC_ARGS: --version 5
; RUN: opt -S -passes=loop-vectorize -mtriple=s390x-linux-gnu -vectorizer-min-trip-count=8 < %s | FileCheck %s

define i32 @main(i32 %arg, ptr nocapture readnone %arg1) #0 {
; CHECK-LABEL: define i32 @main(
; CHECK-SAME: i32 [[ARG:%.*]], ptr nocapture readnone [[ARG1:%.*]]) #[[ATTR0:[0-9]+]] {
; CHECK-NEXT:  [[ENTRY:.*]]:
; CHECK-NEXT:    [[TMP0:%.*]] = alloca i8, align 1
; CHECK-NEXT:    br i1 false, label %[[SCALAR_PH:.*]], label %[[VECTOR_PH:.*]]
; CHECK:       [[VECTOR_PH]]:
; CHECK-NEXT:    br label %[[VECTOR_BODY:.*]]
; CHECK:       [[VECTOR_BODY]]:
; CHECK-NEXT:    [[INDEX:%.*]] = phi i32 [ 0, %[[VECTOR_PH]] ], [ [[INDEX_NEXT:%.*]], %[[VECTOR_BODY]] ]
; CHECK-NEXT:    [[OFFSET_IDX:%.*]] = trunc i32 [[INDEX]] to i8
; CHECK-NEXT:    [[TMP1:%.*]] = add i8 [[OFFSET_IDX]], 0
; CHECK-NEXT:    [[TMP2:%.*]] = add i8 [[OFFSET_IDX]], 1
; CHECK-NEXT:    [[TMP3:%.*]] = add i8 [[OFFSET_IDX]], 2
; CHECK-NEXT:    [[TMP4:%.*]] = add i8 [[OFFSET_IDX]], 3
; CHECK-NEXT:    [[TMP5:%.*]] = add i8 [[OFFSET_IDX]], 4
; CHECK-NEXT:    [[TMP6:%.*]] = add i8 [[OFFSET_IDX]], 5
; CHECK-NEXT:    [[TMP7:%.*]] = add i8 [[OFFSET_IDX]], 6
; CHECK-NEXT:    [[TMP8:%.*]] = add i8 [[OFFSET_IDX]], 7
; CHECK-NEXT:    store i8 [[TMP8]], ptr [[TMP0]], align 2
; CHECK-NEXT:    [[INDEX_NEXT]] = add nuw i32 [[INDEX]], 8
; CHECK-NEXT:    [[TMP9:%.*]] = icmp eq i32 [[INDEX_NEXT]], 8
; CHECK-NEXT:    br i1 [[TMP9]], label %[[MIDDLE_BLOCK:.*]], label %[[VECTOR_BODY]], !llvm.loop [[LOOP0:![0-9]+]]
; CHECK:       [[MIDDLE_BLOCK]]:
; CHECK-NEXT:    br i1 false, label %[[RET:.*]], label %[[SCALAR_PH]]
; CHECK:       [[SCALAR_PH]]:
; CHECK-NEXT:    [[BC_RESUME_VAL:%.*]] = phi i8 [ 8, %[[MIDDLE_BLOCK]] ], [ 0, %[[ENTRY]] ]
; CHECK-NEXT:    br label %[[LOOP:.*]]
; CHECK:       [[LOOP]]:
; CHECK-NEXT:    [[STOREMERGE_I_I:%.*]] = phi i8 [ [[BC_RESUME_VAL]], %[[SCALAR_PH]] ], [ [[TMP12_I_I:%.*]], %[[LOOP]] ]
; CHECK-NEXT:    store i8 [[STOREMERGE_I_I]], ptr [[TMP0]], align 2
; CHECK-NEXT:    [[TMP8_I_I:%.*]] = icmp ult i8 [[STOREMERGE_I_I]], 8
; CHECK-NEXT:    [[TMP12_I_I]] = add nuw nsw i8 [[STOREMERGE_I_I]], 1
; CHECK-NEXT:    br i1 [[TMP8_I_I]], label %[[LOOP]], label %[[RET]], !llvm.loop [[LOOP3:![0-9]+]]
; CHECK:       [[RET]]:
; CHECK-NEXT:    ret i32 0
;
entry:
  %0 = alloca i8, align 1
  br label %loop

loop:
  %storemerge.i.i = phi i8 [ 0, %entry ], [ %tmp12.i.i, %loop ]
  store i8 %storemerge.i.i, ptr %0, align 2
  %tmp8.i.i = icmp ult i8 %storemerge.i.i, 8
  %tmp12.i.i = add nuw nsw i8 %storemerge.i.i, 1
  br i1 %tmp8.i.i, label %loop, label %ret

ret:
  ret i32 0
}

attributes #0 = { "target-cpu"="z13" }

;.
; CHECK: [[LOOP0]] = distinct !{[[LOOP0]], [[META1:![0-9]+]], [[META2:![0-9]+]]}
; CHECK: [[META1]] = !{!"llvm.loop.isvectorized", i32 1}
; CHECK: [[META2]] = !{!"llvm.loop.unroll.runtime.disable"}
; CHECK: [[LOOP3]] = distinct !{[[LOOP3]], [[META2]], [[META1]]}
;.
