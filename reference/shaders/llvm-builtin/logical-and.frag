#version 460

layout(location = 0) in vec4 A;
layout(location = 0) out float SV_Target;

void main()
{
    SV_Target = (isnan(A.x) && isnan(A.y)) ? A.z : A.w;
}


#if 0
// LLVM disassembly
target datalayout = "e-m:e-p:32:32-i1:32-i8:32-i16:32-i32:32-i64:64-f16:32-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

define void @main() {
  %1 = call float @dx.op.loadInput.f32(i32 4, i32 0, i32 0, i8 0, i32 undef)
  %2 = call float @dx.op.loadInput.f32(i32 4, i32 0, i32 0, i8 1, i32 undef)
  %3 = call float @dx.op.loadInput.f32(i32 4, i32 0, i32 0, i8 2, i32 undef)
  %4 = call float @dx.op.loadInput.f32(i32 4, i32 0, i32 0, i8 3, i32 undef)
  %IsNaN = call i1 @dx.op.isSpecialFloat.f32(i32 8, float %1)
  %IsNaN1 = call i1 @dx.op.isSpecialFloat.f32(i32 8, float %2)
  %5 = and i1 %IsNaN, %IsNaN1
  %6 = select i1 %5, float %3, float %4
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %6)
  ret void
}

; Function Attrs: nounwind readnone
declare float @dx.op.loadInput.f32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare i1 @dx.op.isSpecialFloat.f32(i32, float) #0

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!2}
!dx.shaderModel = !{!3}
!dx.typeAnnotations = !{!4}
!dx.viewIdState = !{!8}
!dx.entryPoints = !{!9}

!0 = !{!"dxcoob 2019.05.00"}
!1 = !{i32 1, i32 0}
!2 = !{i32 1, i32 4}
!3 = !{!"ps", i32 6, i32 0}
!4 = !{i32 1, void ()* @main, !5}
!5 = !{!6}
!6 = !{i32 0, !7, !7}
!7 = !{}
!8 = !{[6 x i32] [i32 4, i32 1, i32 1, i32 1, i32 1, i32 1]}
!9 = !{void ()* @main, !"main", !10, null, null}
!10 = !{!11, !14, null}
!11 = !{!12}
!12 = !{i32 0, !"A", i8 9, i8 0, !13, i8 2, i32 1, i8 4, i32 0, i8 0, null}
!13 = !{i32 0}
!14 = !{!15}
!15 = !{i32 0, !"SV_Target", i8 9, i8 16, !13, i8 0, i32 1, i8 1, i32 0, i8 0, null}
#endif
#if 0
// SPIR-V disassembly
; SPIR-V
; Version: 1.3
; Generator: Unknown(30017); 21022
; Bound: 32
; Schema: 0
OpCapability Shader
OpMemoryModel Logical GLSL450
OpEntryPoint Fragment %3 "main" %8 %10
OpExecutionMode %3 OriginUpperLeft
OpName %3 "main"
OpName %8 "A"
OpName %10 "SV_Target"
OpDecorate %8 Location 0
OpDecorate %10 Location 0
%1 = OpTypeVoid
%2 = OpTypeFunction %1
%5 = OpTypeFloat 32
%6 = OpTypeVector %5 4
%7 = OpTypePointer Input %6
%8 = OpVariable %7 Input
%9 = OpTypePointer Output %5
%10 = OpVariable %9 Output
%12 = OpTypePointer Input %5
%13 = OpTypeInt 32 0
%14 = OpConstant %13 0
%17 = OpConstant %13 1
%20 = OpConstant %13 2
%23 = OpConstant %13 3
%26 = OpTypeBool
%3 = OpFunction %1 None %2
%4 = OpLabel
OpBranch %30
%30 = OpLabel
%11 = OpInBoundsAccessChain %12 %8 %14
%15 = OpLoad %5 %11
%16 = OpInBoundsAccessChain %12 %8 %17
%18 = OpLoad %5 %16
%19 = OpInBoundsAccessChain %12 %8 %20
%21 = OpLoad %5 %19
%22 = OpInBoundsAccessChain %12 %8 %23
%24 = OpLoad %5 %22
%25 = OpIsNan %26 %15
%27 = OpIsNan %26 %18
%28 = OpLogicalAnd %26 %25 %27
%29 = OpSelect %5 %28 %21 %24
OpStore %10 %29
OpReturn
OpFunctionEnd
#endif