#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERROR_32 0x69696969
#define NRM "\x1B[0m"
#define RED "\x1B[31m"
#define GRN "\x1B[32m"
#define YEL "\x1B[33m"
#define BLU "\x1B[34m"
#define MAG "\x1B[35m"
#define CYN "\x1B[36m"
#define WHT "\x1B[37m"
#define RES "\033[0m"

inline static int run_calc(FILE *__restrict output);
inline static void get_prescalars(uint32_t exact[], size_t *sz);

typedef struct {
	uint32_t target_freq;
	uint32_t cpu_freq;
} TIMClock;

static TIMClock g_info = {0};

#define PERIOD(PRESC) g_info.cpu_freq / (g_info.target_freq * PRESC);

int main(int argc, char *argv[])
{
	const char *program = argv[0];
	char **args = ++argv;
	if (argc == 1 || argc < 3) {
		fprintf(
		    stderr,
		    "USAGE: %s [cpu_freq(mhz)] [target_freq(hz)] <file.csv>",
		    program);
		return 1;
	}
	int mhz = atol(*(args++));
	g_info.cpu_freq = (mhz * 1000000);
	g_info.target_freq = atol(*(args++));
	printf("cpuclock: %dHz (%d MHz)\ntargetclock: %dHz\n", g_info.cpu_freq,
	       mhz, g_info.target_freq);

	if (argc > 3) {
		const char *file_name = *(args++);
		FILE *file = fopen(file_name, "w");
		if (file == NULL) {
			fprintf(stderr, "ERROR: Open File %s | %s", file_name,
				strerror(errno));
			fclose(file);
			return 1;
		}
		int out = run_calc(file);
		fclose(file);
		return out;
	}

	return run_calc(NULL);
}
inline static int run_calc(FILE *__restrict output)
{
	uint32_t prescalars[USHRT_MAX] = {0};
	size_t len_ep = 0;
	get_prescalars(prescalars, &len_ep);

	if (output == NULL) {
		printf("+----+--------------+--------------+--------------+\n");
		printf("| NO |" BLU "  TARGET FREQ " RES "|" CYN
		       "   PRESCALAR  " RES "|" GRN "    PERIOD    " RES "|\n");
		printf("+----+--------------+--------------+--------------+\n");
	} else {

		fprintf(output, "target_freq,prescalar,period\n");
	}
	for (size_t idx = 0; idx < len_ep; idx++) {
		uint32_t prescalar = prescalars[idx];
		uint32_t period = PERIOD(prescalar);
		if (output == NULL) {
			printf("|%4d|" BLU "%12d  " RES "|" CYN "%12d  " RES
			       "|" GRN "%12d  " RES "|\n",
			       (int)(idx + 1), g_info.target_freq,
			       prescalar - 1, period - 1);
		} else {
			fprintf(output, "%d,%d,%d\n", g_info.target_freq,
				prescalar - 1, period - 1);
		}
	}
	if (output == NULL) {
		printf("+----+--------------+--------------+--------------+\n");
	} else {
		fprintf(output, "\n");
	}
	return 0;
}

inline static void get_prescalars(uint32_t exact[], size_t *sz)
{
	for (int psc = 1; psc < USHRT_MAX; psc++) {
		uint32_t arr = PERIOD(psc);
		if ((0 == (g_info.cpu_freq % psc)) && (USHRT_MAX >= arr)) {
			exact[*sz] = psc;
			(*sz)++;
		}
	}
}
