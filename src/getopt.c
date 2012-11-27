#include "getopt.h"

#include <ctype.h>
#include <stdio.h>  // for vsnprintf
#include <stdarg.h> // for va_list
#include <string.h>

static inline int str_case_cmp_len(const char* s1, const char* s2, unsigned int len)
{
#if defined (_MSC_VER)
	for(unsigned int i = 0; i < len; i++)
	{
		int c1 = tolower(s1[i]);
		int c2 = tolower(s2[i]);
		if(c1 < c2) return -1;
		if(c1 > c2) return  1;
		if(c1 == '\0' && c1 == c2) return 0;
	}
	return 0;
#else // defined (_MSC_VER)
	return strncasecmp(s1, s2, len);
#endif // defined (_MSC_VER)
}

static inline int str_format(char* buf, unsigned int buf_size, const char* fmt, ...)
{
	va_list args;
	va_start( args, fmt );
	int ret = vsnprintf( buf, buf_size, fmt, args );
#if defined(_MSC_VER)
	buf[buf_size - 1] = '\0';
#endif // defined(_MSC_VER)
	va_end( args );
	return ret;
}

enum getopt_result getopt_create(struct getopt* ctx, int argc, char** argv, const struct getopt_option* opts)
{
	ctx->argc = argc - 1; // stripping away file-name!
	ctx->argv = argv + 1; // stripping away file-name!
	ctx->opts = opts;
	ctx->current_index = 0;
	ctx->current_opt_arg = 0x0;

	// count opts
	ctx->num_opts = 0;
	struct getopt_option cmp_opt = { 0 };
	const struct getopt_option* opt = opts;
	while(memcmp(opt, &cmp_opt, sizeof(struct getopt_option)) != 0)
	{
		if(opt->val == '!' || opt->val == '?' || opt->val == '+' || opt->val == 0 || opt->val == -1)
			return GETOPT_RESULT_INVALID_PARAMETER;

		ctx->num_opts++; opt++;
	}

	return GETOPT_RESULT_OK;
}

int getopt_parse(struct getopt* ctx)
{
	unsigned int i;

	// are all options processed?
	if(ctx->current_index == (unsigned int)ctx->argc)
		return -1;

	// reset opt-arg
	ctx->current_opt_arg = 0x0;

	const char* curr_token = ctx->argv[ctx->current_index];
	
	// this token has been processed!
	ctx->current_index++;

	// check if item is no option
	if(curr_token[0] && curr_token[0] != '-')
	{
		ctx->current_opt_arg = curr_token;
		return '+'; // return '+' as identifier for no option!
	}
	
	const struct getopt_option* found_opt = 0x0;
	const char* found_arg = 0x0;

	if(curr_token[1] != '\0' && curr_token[1] != '-' && curr_token[2] == '\0') // short opt
	{
		for(i = 0; i < ctx->num_opts; i++)
		{
			const struct getopt_option* opt = ctx->opts + i;

			if(opt->name_short == curr_token[1])
			{
				found_opt = opt;

				// if there is an value when: - current_index < argc and value in argv[current_index] do not start with '-'
				int has_value = (ctx->current_index != (unsigned int)ctx->argc) && (ctx->argv[ctx->current_index][0] != '-');

				if(has_value && (opt->type == GETOPT_OPTION_TYPE_OPTIONAL || opt->type == GETOPT_OPTION_TYPE_REQUIRED))
				{
					found_arg = ctx->argv[ctx->current_index];
					ctx->current_index++; // next token has been processed aswell!
				}
				break;
			}
		}
	}
	else if(curr_token[1] == '-' && curr_token[2] != '\0') // long opt
	{
		const char* check_option = curr_token + 2;

		for(i = 0; i < ctx->num_opts; i++)
		{
			const struct getopt_option* opt = ctx->opts + i;

			unsigned int name_len = (unsigned int)strlen(opt->name);

			if(str_case_cmp_len(opt->name, check_option, name_len) == 0)
			{
				check_option += name_len;
				found_arg = *check_option == '=' ? check_option + 1 : 0x0;
				found_opt = opt;
				break;
			}
		}
	}
	else // malformed opt "-", "-xyz" or "--"
	{
		ctx->current_opt_arg = curr_token;
		return '!';
	}

	if(found_opt == 0x0) // found no matching option!
	{
		ctx->current_opt_arg = curr_token;
		return '?';
	}

	if(found_arg != 0x0)
	{
		switch(found_opt->type)
		{
			case GETOPT_OPTION_TYPE_FLAG_SET:
			case GETOPT_OPTION_TYPE_FLAG_AND:
			case GETOPT_OPTION_TYPE_FLAG_OR:
			case GETOPT_OPTION_TYPE_NO_ARG:
				// these types should have no argument, usage error!
				ctx->current_opt_arg = found_opt->name;
				return '!';

			case GETOPT_OPTION_TYPE_OPTIONAL:
			case GETOPT_OPTION_TYPE_REQUIRED:
				ctx->current_opt_arg = found_arg;
				return found_opt->val;
		}
	}
	else // no argument found
	{
		switch(found_opt->type)
		{
			case GETOPT_OPTION_TYPE_FLAG_SET: *found_opt->flag  = found_opt->val; return 0;
			case GETOPT_OPTION_TYPE_FLAG_AND: *found_opt->flag &= found_opt->val; return 0;
			case GETOPT_OPTION_TYPE_FLAG_OR:  *found_opt->flag |= found_opt->val; return 0; // zero is "a flag was set!"
			
			case GETOPT_OPTION_TYPE_NO_ARG:
			case GETOPT_OPTION_TYPE_OPTIONAL:
				return found_opt->val;

			case GETOPT_OPTION_TYPE_REQUIRED: // the option requires an argument! (--option=arg, -o arg)
					ctx->current_opt_arg = found_opt->name;
					return '!';
		}
	}

 	return -1;
}

const char* getopt_create_help_string(struct getopt* ctx, char* buffer, unsigned int buffer_size)
{
	int buffer_pos = 0;
	int i = 0;
	for(i = 0; i < ctx->num_opts; ++i)
	{
		const struct getopt_option* opt = ctx->opts + i;

		char long_name[64];
		int outpos = str_format(long_name, 64, "--%s", opt->name);

		if(opt->type == GETOPT_OPTION_TYPE_REQUIRED)
			str_format(long_name + outpos, 64 - outpos, "=<%s>", opt->value_desc);

		if(opt->name_short == 0x0)
			buffer_pos += str_format(buffer + buffer_pos, buffer_size - buffer_pos, "   %-32s - %s\n", long_name, opt->desc);
		else
			buffer_pos += str_format(buffer + buffer_pos, buffer_size - buffer_pos, "-%c %-32s - %s\n", opt->name_short, long_name, opt->desc);
	}

	return buffer;
}
