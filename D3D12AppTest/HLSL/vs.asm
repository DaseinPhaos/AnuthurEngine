//
// Generated by Microsoft (R) HLSL Shader Compiler 10.0.10011.16384
//
//
// Buffer Definitions: 
//
// cbuffer wpMatrix
// {
//
//   float4x4 gWPTransform;             // Offset:    0 Size:    64
//
// }
//
//
// Resource Bindings:
//
// Name                                 Type  Format         Dim      ID      HLSL Bind  Count
// ------------------------------ ---------- ------- ----------- ------- -------------- ------
// wpMatrix                          cbuffer      NA          NA     CB0            cb0      1 
//
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// POSITION                 0   xyzw        0     NONE   float   xyzw
// COLOR                    0   xyzw        1     NONE   float   xyzw
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_POSITION              0   xyzw        0      POS   float   xyzw
// COLOR                    0   xyzw        1     NONE   float   xyzw
//
vs_5_1
dcl_globalFlags refactoringAllowed | skipOptimization
dcl_constantbuffer CB0[0:0][4], immediateIndexed, space=0
dcl_input v0.xyzw
dcl_input v1.xyzw
dcl_output_siv o0.xyzw, position
dcl_output o1.xyzw
dcl_temps 2
//
// Initial variable locations:
//   v0.x <- v.pos.x; v0.y <- v.pos.y; v0.z <- v.pos.z; v0.w <- v.pos.w; 
//   v1.x <- v.color.x; v1.y <- v.color.y; v1.z <- v.color.z; v1.w <- v.color.w; 
//   o1.x <- <VS return value>.color.x; o1.y <- <VS return value>.color.y; o1.z <- <VS return value>.color.z; o1.w <- <VS return value>.color.w; 
//   o0.x <- <VS return value>.pos.x; o0.y <- <VS return value>.pos.y; o0.z <- <VS return value>.pos.z; o0.w <- <VS return value>.pos.w
//
#line 20 "C:\Users\asus\Desktop\vs.hlsl"
mul r0.xyzw, v0.xxxx, CB0[0][0].xyzw
mul r1.xyzw, v0.yyyy, CB0[0][1].xyzw
add r0.xyzw, r0.xyzw, r1.xyzw
mul r1.xyzw, v0.zzzz, CB0[0][2].xyzw
add r0.xyzw, r0.xyzw, r1.xyzw
mul r1.xyzw, v0.wwww, CB0[0][3].xyzw
add r0.xyzw, r0.xyzw, r1.xyzw  // r0.x <- vout.pos.x; r0.y <- vout.pos.y; r0.z <- vout.pos.z; r0.w <- vout.pos.w
mov r1.xyzw, v1.xyzw  // r1.x <- vout.color.x; r1.y <- vout.color.y; r1.z <- vout.color.z; r1.w <- vout.color.w
mov o0.xyzw, r0.xyzw
mov o1.xyzw, r1.xyzw
ret 
// Approximately 11 instruction slots used
