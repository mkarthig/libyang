/**
 * @file common.h
 * @author Radek Krejci <rkrejci@cesnet.cz>
 * @brief common internal definitions for libyang
 *
 * Copyright (c) 2015 CESNET, z.s.p.o.
 *
 * This source code is licensed under BSD 3-Clause License (the "License").
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://opensource.org/licenses/BSD-3-Clause
 */

#ifndef LY_COMMON_H_
#define LY_COMMON_H_

#include <stdint.h>

#include "dict_private.h"
#include "libyang.h"
#include "resolve.h"

#ifdef __GNUC__
#  define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
#else
#  define UNUSED(x) UNUSED_ ## x
#endif

#ifndef __WORDSIZE
#  if defined __x86_64__ && !defined __ILP32__
#   define __WORDSIZE 64
#  else
#   define __WORDSIZE 32
#  endif
#endif

#ifndef __INT64_C
#  if __WORDSIZE == 64
#    define __INT64_C(c)  c ## L
#    define __UINT64_C(c) c ## UL
#  else
#    define __INT64_C(c)  c ## LL
#    define __UINT64_C(c) c ## ULL
#  endif
#endif

/*
 * If the compiler supports attribute to mark objects as hidden, mark all
 * objects as hidden and export only objects explicitly marked to be part of
 * the public API.
 */
#define API __attribute__((visibility("default")))

#ifndef  __USE_GNU
/*
 * If we don't have GNU extension, implement these function on your own
 */
char *get_current_dir_name(void);

#endif

#define LY_BUF_SIZE 1024
#define LY_APPTAG_LEN 128
struct ly_err {
    LY_ERR no;
    LY_VECODE code;
    uint8_t vlog_hide;
    uint8_t buf_used;
    uint16_t path_index;
    uint8_t path_obj_type;
    const void *path_obj;
    char msg[LY_BUF_SIZE];
    char path[LY_BUF_SIZE];
    char apptag[LY_APPTAG_LEN];
    char buf[LY_BUF_SIZE];
};

/**
 * @brief libyang internal thread-specific buffer of LY_BUF_SIZE size
 *
 * Caller is responsible to check and set ly_buf_used to 1 (and set
 * buf[0] to '\0') when starts to use buffer and back to 0 when the
 * buffer is no more needed. If the buffer is already used, it is
 * possible to duplicate the buffer content and write string back to
 * the buffer when leaving.
 */
uint8_t *ly_buf_used_location(void);
char *ly_buf(void);
#define ly_buf_used (*ly_buf_used_location())

/*
 * logger
 */
extern volatile int8_t ly_log_level;

void ly_log(LY_LOG_LEVEL level, const char *format, ...);

#define LOGERR(errno, str, args...)                                 \
    if (errno) { ly_errno = errno; }                                \
    if (ly_log_level >= LY_LLERR) {                                 \
        ly_log(LY_LLERR, str, ##args);                              \
    }

#define LOGWRN(str, args...)                                        \
	if (ly_log_level >= LY_LLWRN) {                                 \
		ly_log(LY_LLWRN, str, ##args);                              \
	}

#define LOGVRB(str, args...)                                        \
	if (ly_log_level >= LY_LLVRB) {                                 \
		ly_log(LY_LLVRB, str, ##args);                              \
	}

#ifdef NDEBUG
#define LOGDBG(str, args...)
#else
#define LOGDBG(str, args...)                                        \
	if (ly_log_level >= LY_LLDBG) {                                 \
		ly_log(LY_LLDBG, str, ##args);                              \
	}
#endif

#define LOGMEM LOGERR(LY_EMEM, "Memory allocation failed (%s()).", __func__)

#define LOGINT LOGERR(LY_EINT, "Internal error (%s:%d).", __FILE__, __LINE__)

typedef enum {
    LYE_PATH = -2,    /**< error path set */
    LYE_SPEC = -1,    /**< generic error */

    LYE_SUCCESS = 0,

    LYE_XML_MISS,
    LYE_XML_INVAL,
    LYE_XML_INCHAR,

    LYE_EOF,
    LYE_INSTMT,
    LYE_INCHILDSTMT,
    LYE_INPAR,
    LYE_INID,
    LYE_INDATE,
    LYE_INARG,
    LYE_MISSSTMT,
    LYE_MISSCHILDSTMT,
    LYE_MISSARG,
    LYE_TOOMANY,
    LYE_DUPID,
    LYE_DUPLEAFLIST,
    LYE_DUPLIST,
    LYE_NOUNIQ,
    LYE_ENUM_INVAL,
    LYE_ENUM_INNAME,
    LYE_ENUM_DUPVAL,
    LYE_ENUM_DUPNAME,
    LYE_ENUM_WS,
    LYE_BITS_INVAL,
    LYE_BITS_INNAME,
    LYE_BITS_DUPVAL,
    LYE_BITS_DUPNAME,
    LYE_INMOD,
    LYE_INMOD_LEN,
    LYE_KEY_NLEAF,
    LYE_KEY_TYPE,
    LYE_KEY_CONFIG,
    LYE_KEY_MISS,
    LYE_KEY_DUP,
    LYE_INREGEX,
    LYE_INRESOLV,
    LYE_INSTATUS,
    LYE_CIRC_LEAFREFS,
    LYE_CIRC_FEATURES,
    LYE_CIRC_IMPORTS,
    LYE_CIRC_INCLUDES,
    LYE_INVER,

    LYE_OBSDATA,
    LYE_OBSTYPE,
    LYE_NORESOLV,
    LYE_INELEM,
    LYE_INELEM_LEN,
    LYE_MISSELEM,
    LYE_INVAL,
    LYE_INVALATTR,
    LYE_INATTR,
    LYE_MISSATTR,
    LYE_NOCONSTR,
    LYE_INCHAR,
    LYE_INPRED,
    LYE_MCASEDATA,
    LYE_NOMUST,
    LYE_NOWHEN,
    LYE_INORDER,
    LYE_INWHEN,
    LYE_NOMIN,
    LYE_NOMAX,
    LYE_NOREQINS,
    LYE_NOLEAFREF,
    LYE_NOMANDCHOICE,

    LYE_XPATH_INSNODE,
    LYE_XPATH_INTOK,
    LYE_XPATH_EOF,
    LYE_XPATH_INOP_1,
    LYE_XPATH_INOP_2,
    LYE_XPATH_INCTX,
    LYE_XPATH_INARGCOUNT,
    LYE_XPATH_INARGTYPE,
    LYE_XPATH_DUMMY,

    LYE_PATH_INCHAR,
    LYE_PATH_INMOD,
    LYE_PATH_MISSMOD,
    LYE_PATH_INNODE,
    LYE_PATH_INKEY,
    LYE_PATH_MISSKEY,
    LYE_PATH_EXISTS,
    LYE_PATH_MISSPAR,
} LY_ECODE;

enum LY_VLOG_ELEM {
    LY_VLOG_NONE = 0,
    LY_VLOG_XML, /* struct lyxml_elem* */
    LY_VLOG_LYS, /* struct lys_node* */
    LY_VLOG_LYD, /* struct lyd_node* */
    LY_VLOG_STR  /* const char* */
};
void ly_vlog_hide(int hide);
uint8_t *ly_vlog_hide_location(void);
void ly_vlog(LY_ECODE code, enum LY_VLOG_ELEM elem_type, const void *elem, ...);
#define LOGVAL(code, elem_type, elem, args...) ly_vlog(code, elem_type, elem, ##args)
#define LOGPATH(elem_type, elem) ly_vlog(LYE_PATH, elem_type, elem)

void ly_vlog_build_path_reverse(enum LY_VLOG_ELEM elem_type, const void *elem, char *path, uint16_t *index);

struct lys_module *ly_ctx_load_sub_module(struct ly_ctx *ctx, struct lys_module *module, const char *name,
                                          const char *revision, int implement, struct unres_schema *unres);

/**
 * @brief Basic functionality like strpbrk(3). However, it searches string \p s
 *        backwards up to most \p s_len characters.
 *
 * @param[in] s String to search backwards.
 * @param[in] accept String of characters that are searched for.
 * @param[in] s_len Backward length of \p s.
 *
 * @return Pointer to the first backward occurence of a character from
 *         \p accept or \p s - \p s_len if not found.
 */
const char *strpbrk_backwards(const char *s, const char *accept, unsigned int s_len);

char *strnchr(const char *s, int c, unsigned int len);

const char *strnodetype(LYS_NODE type);

/**
 * @brief Transform a module name (JSON format prefix) to a prefix as defined
 * in \p module imports. Its own name is transformed to its own prefix.
 *
 * @param[in] module Module with imports to use.
 * @param[in] module_name Module name to transform.
 *
 * @return Module import prefix (do not modify, free, or lydict_remove),
 * NULL on error.
 */
const char *transform_module_name2import_prefix(const struct lys_module *module, const char *module_name);

/**
 * @brief Transform expression from JSON format to XML format.
 * Output arrays point to strings in the dictionary, but without
 * correcting their ref_count -> do not touch them. Prefixes of
 * the namespaces are prefixes specified by the module itself. Output
 * parameters are optional, but either all 3 are set or none
 * of them are. Logs directly.
 *
 * @param[in] module Module with imports to use.
 * @param[in] expr JSON expression.
 * @param[out] prefixes Array of pointers to prefixes. After use free them with free(*prefixes).
 * Can be NULL.
 * @param[out] namespaces Array of pointers to full namespaces. After use free them with
 * free(*namespaces). Can be NULL.
 * @param[out] ns_count Number of elements in both \p prefixes and \p namespaces arrays.
 * Can be NULL.
 *
 * @return Transformed XML expression in the dictionary, NULL on error.
 */
const char *transform_json2xml(const struct lys_module *module, const char *expr, const char ***prefixes, const char ***namespaces,
                               uint32_t *ns_count);

/**
 * @brief Transform expression from JSON format to schema format.
 * Prefixes of the namespaces (models) are those from the main
 * \p module imports of the corresponding modules. Logs directly.
 *
 * @param[in] module Module with imports to use.
 * @param[in] expr JSON expression.
 *
 * @return Transformed XML expression in the dictionary, NULL on error.
 */
const char *transform_json2schema(const struct lys_module *module, const char *expr);

/**
 * @brief Transform expression from XML data format (prefixes and separate NS definitions) to
 *        JSON format (prefixes are module names instead). Logs directly.
 *
 * @param[in] ctx libyang context to use.
 * @param[in] expr XML expression.
 * @param[in] xml XML element with the expression.
 * @param[in] log Whether to log errors or not.
 *
 * @return Transformed JSON expression in the dictionary, NULL on error.
 */
const char *transform_xml2json(struct ly_ctx *ctx, const char *expr, struct lyxml_elem *xml, int log);

/**
 * @brief Transform expression from the schema format (prefixes of imports) to
 *        JSON format (prefixes are module names directly). Logs directly.
 *
 * @param[in] module Module (schema) with imports to search.
 * @param[in] expr Expression from \p module.
 *
 * @return Transformed JSON expression in the dictionary, NULL on error.
 */
const char *transform_schema2json(const struct lys_module *module, const char *expr);

/**
 * @brief Wrapper for realloc() call. The only difference is that if it fails to
 * allocate the requested memory, the original memory is freed as well.
 *
 * @param[in] ptr Memory to reallocate.
 * @param[in] size New size of the memory block.
 *
 * @return Pointer to the new memory, NULL on error.
 */
void *ly_realloc(void *ptr, size_t size);

/**
 * @brief Compare strings
 * @param[in] s1 First string to compare
 * @param[in] s2 Second string to compare
 * @param[in] both_in_dictionary Flag for optimization, 1 if it is sure that \p s1 and \p s2 were stored in dictionary.
 * This parameter is supposed to be a number (digit) known in compile time, not a variable or expression!
 * @return 1 if both strings are the same, 0 if they differ.
 */
int ly_strequal_(const char *s1, const char *s2);
#define ly_strequal0(s1, s2) ly_strequal_(s1, s2)
#define ly_strequal1(s1, s2) (s1 == s2)
#define ly_strequal(s1, s2, d) ly_strequal##d(s1, s2)

int64_t dec_pow(uint8_t exp);

int dec64cmp(int64_t num1, uint8_t dig1, int64_t num2, uint8_t dig2);

#endif /* LY_COMMON_H_ */
