.arch armv7-a
.section .text

.global taiHookFunctionImport
.type taiHookFunctionImport, %function
taiHookFunctionImport:
    bx lr

.global taiHookRelease
.type taiHookRelease, %function
taiHookRelease:
    bx lr
