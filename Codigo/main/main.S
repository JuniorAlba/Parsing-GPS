    .section .text
    .align 4
    .global assembly_sum

assembly_sum:
    # Cargar los dos valores a sumar
    li a0, 25        # primer número
    li a1, 17        # segundo número

    # Sumar los valores
    add a0, a0, a1   # a0 = a0 + a1

    # Devolver resultado (por convención de RISC-V, el retorno va en a0)
    ret