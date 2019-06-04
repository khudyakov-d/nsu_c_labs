	.file	"lr3.c"
	.text
	.globl	ex
	.type	ex, @function
ex:
.LFB0:
	.cfi_startproc	//задаёт начало процедуры
					//устанавливает начальный регистр и смещение
					//для расчёта адреса CFA (Canonical Frame Address).
	
	pushl	%ebp	//сохраняет текущий указатель кадра стека	
	.cfi_def_cfa_offset 8  //обновляет смещение CFA, говоря, 
							//что оно стало равно 8 относительно заданного
							// (директивой .cfi_startproc) регистра (%rsp).
	.cfi_offset 5, -8 	//говорит, что теперь регистр 5 лежит по смещению
						//-8 от CFA (таким образом была описана инструкция pushq %rbp).	
	movl	%esp, %ebp //устанавливает новый стековый кадр		
						//копирует адрес регистра esp в ebp
	.cfi_def_	cfa_register 5	// говорит, что теперь для расчёта 
							//адреса CFA используется регистр 5.
	subl	$48, %esp // отнимаем из esp значение 48 
					//резервация места под локальные переменные
	call	__x86.get_pc_thunk.ax
	addl	$_GLOBAL_OFFSET_TABLE_, %eax
	fld1 //загружаем на стек FPU (математический сопроцессор) единицу (double exVal= 1)
	fstpl	-16(%ebp) //копируем вершину стека FPU по адресу -16(%ebp) и выталкиваем это значение
	fld1	//загружаем на стек FPU (математический сопроцессор) единицу (double el = 1)
	fstpl	-8(%ebp) //копируем вершину стека FPU по адресу -8(%ebp) и выталкиваем это значение 
	movl	$1, -20(%ebp) // кладем по адресу -20(%ebp) единицу (size_t i)

 	jmp	.L2
.L3:
	fildl	8(%ebp) // загружаем в стек сопроцессора целое число расположенного по адресу 8(%ebp)
					// видимо это наш аргумент x т.к. обращаемся по положительному адресу
	fmull	-8(%ebp) // x*el и кладем результат в сопроцессор
	movl	-20(%ebp), %eax //копируем i в регистр %eax 
	movl	$0, %edx // кладем в регистр edx 0
	movl	%eax, -48(%ebp) // кладем i по адресу -48(%ebp) 
	movl	%edx, -44(%ebp) // кладем 0 по адресу -44(%ebp)
	fildq	-48(%ebp) // загружаем целое i в стек FPU  
	fstpl	-40(%ebp) //копируем вершину стека FPU по адресу -40(%ebp) (кладем  i) и выталкиваем это значение 
	fldl	-40(%ebp) //загружаем на стек FPU (математический сопроцессор) i как вещественное число 
	fdivrp	%st, %st(1) //делим x*el на i и кладем результат на стек сопроцессора
	fstpl	-8(%ebp) // копируем вершину стека FPU по адресу -8(%ebp) (кладем x*el/i) и выталкиваем это значение
	fldl	-16(%ebp) // загружаем exVal на стек FPU 
	faddl	-8(%ebp) // Прибавляем к вершине стека FPU значение по адресу -8(%ebp) (exVal += x*el/i)
	fstpl	-16(%ebp) // Копируем вершину стека FPU по адресу -16(%ebp) (кладем новое значение exVal) и выталкиваем это значение
	addl	$1, -20(%ebp) // добавить к i единицу
.L2:
	cmpl	$499999999, -20(%ebp) // сравнение 
	jbe	.L3 //Команда JBE выполняет короткий переход, 
			//первый операнд МЕНЬШЕ или РАВЕН второму операнду
			// при выполнении операции сравнения с помощью команды CMP.
	fldl	-16(%ebp) //загружаем на стек FPU (математический сопроцессор) -16(%ebp) (return exVal)
	
	leave //восстанавливаем кадр стека (копируем в ebp в esp)
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	ex, .-ex
	.globl	main
	.type	main, @function
main:
.LFB1:
	.cfi_startproc
	leal	4(%esp), %ecx 
	.cfi_def_cfa 1, 0
	andl	$-8, %esp
	pushl	-4(%ecx) //?
	pushl	%ebp //сохраняет текущий указатель кадра стека	
	.cfi_escape 0x10,0x5,0x2,0x75,0
	movl	%esp, %ebp //устанавливает новый стековый кадр		
	
						//копирует адрес регистра esp в ebp
	pushl	%ecx 
	.cfi_escape 0xf,0x3,0x75,0x7c,0x6
	subl	$20, %esp //отнимаем от esp 20 , резервируем место под локальные переменные 
	call	__x86.get_pc_thunk.ax
	addl	$_GLOBAL_OFFSET_TABLE_, %eax
	movl	$10, -20(%ebp) //кладем 10 по адресу -20(%ebp) (x = 10)
	subl	$4, %esp//
	pushl	-20(%ebp) // кладем на стек значение по адресу -20(%ebp)
	call	ex // вызываем функцию ex и кладем на стек адрес возврата
	addl	$8, %esp // прибавляем 8 к %esp и кладем результат на стек сопроцессора?
	fstpl	-16(%ebp) // снимаем результат функции со стека сопроцессора и кладем по адресу -16(%ebp)
	movl	$0, %eax
	movl	-4(%ebp), %ecx
	.cfi_def_cfa 1, 0
	leave
	.cfi_restore 5
	leal	-4(%ecx), %esp
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE1:
	.size	main, .-main
	.section	.text.__x86.get_pc_thunk.ax,"axG",@progbits,__x86.get_pc_thunk.ax,comdat
	.globl	__x86.get_pc_thunk.ax
	.hidden	__x86.get_pc_thunk.ax
	.type	__x86.get_pc_thunk.ax, @function
__x86.get_pc_thunk.ax:
.LFB2:
	.cfi_startproc
	movl	(%esp), %eax
	ret
	.cfi_endproc
.LFE2:
	.ident	"GCC: (Ubuntu 7.3.0-27ubuntu1~18.04) 7.3.0"
	.section	.note.GNU-stack,"",@progbits
