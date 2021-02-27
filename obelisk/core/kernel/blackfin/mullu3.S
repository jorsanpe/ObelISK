.align 2
.global ___mullu3;
.type ___mullu3, STT_FUNC;

#ifdef CONFIG_ARITHMETIC_OPS_L1
.section .l1.text
#else
.text
#endif

___mullu3:
	// We are using normal stores rather than pushes to allow
	// multi-issuing, but since they are post- rather than pre-decrement,
	// we first need to decrement SP to prevent clobbering half of the
	// second operand, that is on the top of the stack.
	SP += -4;
    R5=(A1 = R0.L*R2.L) (FU) || [SP--] = R5;
    R5 = R5.L (Z);
    A1 = A1 >> 16;
    A1 += R0.H * R2.L (FU) || [SP--] = R6;
    A1 += R0.L * R2.H (FU) || R3 = [SP+24];
    R6 = A1.W;
    R6 = R6 << 16;
    A1 = A1 >> 16;
    R6 = R6 | R5;
    A1 += R3.L * R0.L (FU) || [SP] = R7;
    A1 += R2.L * R1.L (FU);
    A1 += R0.H * R2.H (FU);
    R5 = A1.W;
    R5 = R5.L (Z);
    A1 = A1 >> 16;
    A1 += R1.H * R2.L (FU);
    A1 += R3.H * R0.L (FU);
    A1 += R1.L * R2.H (FU);
    A1 += R3.L * R0.H (FU) || R7 = [SP++];
    R3 = A1.W;
    R0 = R6;
    R3 = R3 << 16 || R6 = [SP++];
    R1 = R5 | R3;
    R5 = [SP++];
    RTS;

.size ___mullu3, .-___mullu3
