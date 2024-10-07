; RUN: not --crash llc -mtriple=stubv32 < %s 2>&1 | FileCheck %s

target datalayout = "e-m:e-p:32:32-i64:64-n32-S128"
target triple = "stubv32"

; FIXME: We currently can't select constants unless they fit in the
; register/immediate instructions that use them
; CHECK: Cannot select:{{.*}} i32 = Constant<2049>
define i32 @add(i32 %a) {
  %b = add i32 %a, 2049
  ret i32 %b
}

