#include "x64.h"

void _cgSwitchRoutine() {
      fprintf(Outfile,
	  "# internal switch(expr) routine\n"
	  "# %%rsi = switch table, %%rax = expr\n"
	  "# from SubC: http://www.t3x.org/subc/\n"
	  "\n"
	  "__switch:\n"
	  "        pushq   %%rsi\n"
	  "        movq    %%rdx, %%rsi\n"
	  "        movq    %%rax, %%rbx\n"
	  "        cld\n"
	  "        lodsq\n"
	  "        movq    %%rax, %%rcx\n"
	  "__next:\n"
	  "        lodsq\n"
	  "        movq    %%rax, %%rdx\n"
	  "        lodsq\n"
	  "        cmpq    %%rdx, %%rbx\n"
	  "        jnz     __no\n"
	  "        popq    %%rsi\n"
	  "        jmp     *%%rax\n"
	  "__no:\n"
	  "        loop    __next\n"
	  "        lodsq\n"
	  "        popq    %%rsi\n"
	  "        jmp     *%%rax\n\n");
}

// Generate a switch jump table and the code to
// load the registers and call the switch() code
void cgSwitch(int reg, int casecount, int toplabel,
              int *caselabel, int *caseval, int defaultlabel)
{
    int i, label;

    // Get a label for the switch table
    label = getLabel();
    cgOutputLabel(label);

    // Heuristic. If we have no cases, create one case
    // which points to the default case
    if (casecount == 0)
    {
        caseval[0] = 0;
        caselabel[0] = defaultlabel;
        casecount = 1;
    }
    // Generate the switch jump table.
    fprintf(Outfile, "\t.quad\t%d\n", casecount);
    for (i = 0; i < casecount; i++)
        fprintf(Outfile, "\t.quad\t%d, L%d\n", caseval[i], caselabel[i]);
    fprintf(Outfile, "\t.quad\tL%d\n", defaultlabel);

    // Load the specific registers
    cgOutputLabel(toplabel);
    fprintf(Outfile, "\tmovq\t%s, %%rax\n", registerList[reg]);
    fprintf(Outfile, "\tleaq\tL%d(%%rip), %%rdx\n", label);
    fprintf(Outfile, "\tjmp\t__switch\n");
}
