; RUN: not --crash llc -mtriple=stubv32 < %s 2>&1 | FileCheck %s

target datalayout = "e-m:e-p:32:32-i64:64-n32-S128"
target triple = "stubv32"

; FIXME: We currently can't select constants unless they fit in the
; register/immediate instructions that use them
; CHECK: Cannot select:{{.*}} i32 = Constant<2048>
define i32 @xor(i32 %a) {
  %b = xor i32 %a, 2048
  ret i32 %b
}
