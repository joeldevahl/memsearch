#ifndef GETOPT_H_INCLUDED
#define GETOPT_H_INCLUDED

enum getopt_result
{
	GETOPT_RESULT_OK,
	GETOPT_RESULT_INVALID_PARAMETER
};

enum getopt_option_type
{
	GETOPT_OPTION_TYPE_NO_ARG,
	GETOPT_OPTION_TYPE_REQUIRED,
	GETOPT_OPTION_TYPE_OPTIONAL,
	GETOPT_OPTION_TYPE_FLAG_SET,
	GETOPT_OPTION_TYPE_FLAG_AND,
	GETOPT_OPTION_TYPE_FLAG_OR
};

struct getopt_option
{
	const char*             name;
	int                     name_short;
	enum getopt_option_type type;
	int*                    flag;
	int                     val;
	const char*             desc;
	const char*             value_desc;
};

struct getopt
{
	int                         argc;
	char**                      argv;
	const struct getopt_option* opts;
	unsigned int                num_opts;
	unsigned int                current_index;
	const char*                 current_opt_arg;
};

enum getopt_result getopt_create(struct getopt* ctx, int argc, char** argv, const struct getopt_option* opts);

int getopt_parse(struct getopt* ctx);

const char* getopt_create_help_string(struct getopt* ctx, char* buffer, unsigned int buffer_size);

#endif // GETOPT_H_INCLUDED
