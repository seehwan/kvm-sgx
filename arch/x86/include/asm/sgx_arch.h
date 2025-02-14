/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */
/**
 * Copyright(c) 2016-18 Intel Corporation.
 *
 * Contains data structures defined by the SGX architecture.  Data structures
 * defined by the Linux software stack should not be placed here.
 */
#ifndef _ASM_X86_SGX_ARCH_H
#define _ASM_X86_SGX_ARCH_H

#include <linux/bits.h>
#include <linux/types.h>

#define SGX_CPUID				0x12
#define SGX_CPUID_FIRST_VARIABLE_SUB_LEAF	2

/**
 * enum sgx_sub_leaf_types - SGX CPUID variable sub-leaf types
 * %SGX_CPUID_SUB_LEAF_INVALID:		Indicates this sub-leaf is invalid.
 * %SGX_CPUID_SUB_LEAF_EPC_SECTION:	Sub-leaf enumerates an EPC section.
 * %SGX_CPUID_SUB_LEAF_TYPE_MASK:	Mask for bits containing the type.
 */
enum sgx_sub_leaf_types {
	SGX_CPUID_SUB_LEAF_INVALID	= 0x0,
	SGX_CPUID_SUB_LEAF_EPC_SECTION	= 0x1,
	SGX_CPUID_SUB_LEAF_TYPE_MASK	= GENMASK(3, 0),
};

/**
 * enum sgx_encls_leaves - ENCLS leaf functions
 * %SGX_ECREATE:	Create an enclave.
 * %SGX_EADD:		Add a page to an uninitialized enclave.
 * %SGX_EINIT:		Initialize an enclave, i.e. launch an enclave.
 * %SGX_EREMOVE:	Remove a page from an enclave.
 * %SGX_EDBGRD:		Read a word from an enclve (peek).
 * %SGX_EDBGWR:		Write a word to an enclave (poke).
 * %SGX_EEXTEND:	Measure 256 bytes of an added enclave page.
 * %SGX_ELDB:		Load a swapped page in blocked state.
 * %SGX_ELDU:		Load a swapped page in unblocked state.
 * %SGX_EBLOCK:		Change page state to blocked i.e. entering hardware
 *			threads cannot access it and create new TLB entries.
 * %SGX_EPA:		Create a Version Array (VA) page used to store isvsvn
 *			number for a swapped EPC page.
 * %SGX_EWB:		Swap an enclave page to the regular memory. Checks that
 *			all threads have exited that were in the previous
 *			shoot-down sequence.
 * %SGX_ETRACK:		Start a new shoot down sequence. Used to together with
 *			EBLOCK to make sure that a page is safe to swap.
 * %SGX_EAUG:		Add a page to an initialized enclave.
 * %SGX_EMODPR:		Restrict an EPC page's permissions.
 * %SGX_EMODT:		Modify the page type of an EPC page.
 */
enum sgx_encls_leaves {
	SGX_ECREATE	= 0x00,
	SGX_EADD	= 0x01,
	SGX_EINIT	= 0x02,
	SGX_EREMOVE	= 0x03,
	SGX_EDGBRD	= 0x04,
	SGX_EDGBWR	= 0x05,
	SGX_EEXTEND	= 0x06,
	SGX_ELDB	= 0x07,
	SGX_ELDU	= 0x08,
	SGX_EBLOCK	= 0x09,
	SGX_EPA		= 0x0A,
	SGX_EWB		= 0x0B,
	SGX_ETRACK	= 0x0C,
	SGX_EAUG	= 0x0D,
	SGX_EMODPR	= 0x0E,
	SGX_EMODT	= 0x0F,
};

#define SGX_MODULUS_SIZE 384

/**
 * enum sgx_miscselect - additional information to an SSA frame
 * %SGX_MISC_EXINFO:	Report #PF or #GP to the SSA frame.
 *
 * Save State Area (SSA) is a stack inside the enclave used to store processor
 * state when an exception or interrupt occurs. This enum defines additional
 * information stored to an SSA frame.
 */
enum sgx_miscselect {
	SGX_MISC_EXINFO		= BIT(0),
	SGX_MISC_RESERVED_MASK	= GENMASK_ULL(63, 1)
};

#define SGX_SSA_GPRS_SIZE		182
#define SGX_SSA_MISC_EXINFO_SIZE	16

/**
 * enum sgx_attributes - the attributes field in &struct sgx_secs
 * %SGX_ATTR_INIT:		Enclave can be entered (is initialized).
 * %SGX_ATTR_DEBUG:		Allow ENCLS(EDBGRD) and ENCLS(EDBGWR).
 * %SGX_ATTR_MODE64BIT:		Tell that this a 64-bit enclave.
 * %SGX_ATTR_PROVISIONKEY:      Allow to use provisioning keys for remote
 *				attestation.
 * %SGX_ATTR_KSS:		Allow to use key separation and sharing (KSS).
 * %SGX_ATTR_EINITTOKENKEY:	Allow to use token signing key that is used to
 *				sign cryptographic tokens that can be passed to
 *				EINIT as an authorization to run an enclave.
 */
enum sgx_attribute {
	SGX_ATTR_INIT		= BIT(0),
	SGX_ATTR_DEBUG		= BIT(1),
	SGX_ATTR_MODE64BIT	= BIT(2),
	SGX_ATTR_PROVISIONKEY	= BIT(4),
	SGX_ATTR_EINITTOKENKEY	= BIT(5),
	SGX_ATTR_KSS		= BIT(7),
	SGX_ATTR_RESERVED_MASK  = BIT_ULL(3) |
				  BIT_ULL(7) |
				  GENMASK_ULL(63, 8),
	SGX_ATTR_ALLOWED_MASK	= SGX_ATTR_DEBUG |
				  SGX_ATTR_MODE64BIT |
				  SGX_ATTR_KSS,
};

#define SGX_SECS_RESERVED1_SIZE 24
#define SGX_SECS_RESERVED2_SIZE 32
#define SGX_SECS_RESERVED3_SIZE 96
#define SGX_SECS_RESERVED4_SIZE 3836

/**
 * struct sgx_secs - SGX Enclave Control Structure (SECS)
 * @size:		size of the address space
 * @base:		base address of the  address space
 * @ssa_frame_size:	size of an SSA frame
 * @miscselect:		additional information stored to an SSA frame
 * @attributes:		attributes for enclave
 * @xfrm:		XSave-Feature Request Mask (subset of XCR0)
 * @mrenclave:		SHA256-hash of the enclave contents
 * @mrsigner:		SHA256-hash of the public key used to sign the SIGSTRUCT
 * @isvprodid:		a user-defined value that is used in key derivation
 * @isvsvn:		a user-defined value that is used in key derivation
 *
 * SGX Enclave Control Structure (SECS) is a special enclave page that is not
 * visible in the address space. In fact, this structure defines the address
 * range and other global attributes for the enclave and it is the first EPC
 * page created for any enclave. It is moved from a temporary buffer to an EPC
 * by the means of ENCLS(ECREATE) leaf.
 */
struct sgx_secs {
	u64 size;
	u64 base;
	u32 ssa_frame_size;
	u32 miscselect;
	u8  reserved1[SGX_SECS_RESERVED1_SIZE];
	u64 attributes;
	u64 xfrm;
	u32 mrenclave[8];
	u8  reserved2[SGX_SECS_RESERVED2_SIZE];
	u32 mrsigner[8];
	u8  reserved3[SGX_SECS_RESERVED3_SIZE];
	u16 isvprodid;
	u16 isvsvn;
	u8  reserved4[SGX_SECS_RESERVED4_SIZE];
} __packed;

/**
 * enum sgx_tcs_flags - execution flags for TCS
 * %SGX_TCS_DBGOPTIN:	If enabled allows single-stepping and breakpoints
 *			inside an enclave. It is cleared by EADD but can
 *			be set later with EDBGWR.
 */
enum sgx_tcs_flags {
	SGX_TCS_DBGOPTIN	= 0x01,
	SGX_TCS_RESERVED_MASK	= GENMASK_ULL(63, 1)
};

#define SGX_TCS_RESERVED_SIZE 4024

/**
 * struct sgx_tcs - Thread Control Structure (TCS)
 * @state:		used to mark an entered TCS
 * @flags:		execution flags (cleared by EADD)
 * @ssa_offset:		SSA stack offset relative to the enclave base
 * @ssa_index:		the current SSA frame index (cleard by EADD)
 * @nr_ssa_frames:	the number of frame in the SSA stack
 * @entry_offset:	entry point offset relative to the enclave base
 * @exit_addr:		address outside the enclave to exit on an exception or
 *			interrupt
 * @fs_offset:		offset relative to the enclave base to become FS
 *			segment inside the enclave
 * @gs_offset:		offset relative to the enclave base to become GS
 *			segment inside the enclave
 * @fs_limit:		size to become a new FS-limit (only 32-bit enclaves)
 * @gs_limit:		size to become a new GS-limit (only 32-bit enclaves)
 *
 * Thread Control Structure (TCS) is an enclave page visible in its address
 * space that defines an entry point inside the enclave. A thread enters inside
 * an enclave by supplying address of TCS to ENCLU(EENTER). A TCS can be entered
 * by only one thread at a time.
 */
struct sgx_tcs {
	u64 state;
	u64 flags;
	u64 ssa_offset;
	u32 ssa_index;
	u32 nr_ssa_frames;
	u64 entry_offset;
	u64 exit_addr;
	u64 fs_offset;
	u64 gs_offset;
	u32 fs_limit;
	u32 gs_limit;
	u8  reserved[SGX_TCS_RESERVED_SIZE];
} __packed;

/**
 * struct sgx_pageinfo - an enclave page descriptor
 * @addr:	address of the enclave page
 * @contents:	pointer to the page contents
 * @metadata:	pointer either to a SECINFO or PCMD instance
 * @secs:	address of the SECS page
 */
struct sgx_pageinfo {
	u64 addr;
	u64 contents;
	u64 metadata;
	u64 secs;
} __packed __aligned(32);


/**
 * enum sgx_page_type - bits in the SECINFO flags defining the page type
 * %SGX_PAGE_TYPE_SECS:	a SECS page
 * %SGX_PAGE_TYPE_TCS:	a TCS page
 * %SGX_PAGE_TYPE_REG:	a regular page
 * %SGX_PAGE_TYPE_VA:	a VA page
 * %SGX_PAGE_TYPE_TRIM:	a page in trimmed state
 */
enum sgx_page_type {
	SGX_PAGE_TYPE_SECS	= 0,
	SGX_PAGE_TYPE_TCS	= 1,
	SGX_PAGE_TYPE_REG	= 2,
	SGX_PAGE_TYPE_VA	= 3,
	SGX_PAGE_TYPE_TRIM	= 4,
	SGX_PAGE_TYPE_MASK	= GENMASK(7, 0)
};

/**
 * enum sgx_secinfo_flags - the flags field in &struct sgx_secinfo
 * %SGX_SECINFO_R:	allow read
 * %SGX_SECINFO_W:	allow write
 * %SGX_SECINFO_X:	allow execution
 * %SGX_SECINFO_SECS:	a SECS page
 * %SGX_SECINFO_TCS:	a TCS page
 * %SGX_SECINFO_REG:	a regular page
 * %SGX_SECINFO_VA:	a VA page
 * %SGX_SECINFO_TRIM:	a page in trimmed state
 */
enum sgx_secinfo_flags {
	SGX_SECINFO_R			= BIT(0),
	SGX_SECINFO_W			= BIT(1),
	SGX_SECINFO_X			= BIT(2),
	SGX_SECINFO_PERMISSION_MASK	= GENMASK_ULL(2, 0),
	SGX_SECINFO_SECS		= (SGX_PAGE_TYPE_SECS << 8),
	SGX_SECINFO_TCS			= (SGX_PAGE_TYPE_TCS << 8),
	SGX_SECINFO_REG			= (SGX_PAGE_TYPE_REG << 8),
	SGX_SECINFO_VA			= (SGX_PAGE_TYPE_VA << 8),
	SGX_SECINFO_TRIM		= (SGX_PAGE_TYPE_TRIM << 8),
	SGX_SECINFO_PAGE_TYPE_MASK	= (SGX_PAGE_TYPE_MASK << 8),
	SGX_SECINFO_RESERVED_MASK	= ~(SGX_SECINFO_PERMISSION_MASK |
					    SGX_SECINFO_PAGE_TYPE_MASK)
};

#define SGX_SECINFO_RESERVED_SIZE 56

/**
 * struct sgx_secinfo - describes attributes of an EPC page
 * @flags:	permissions and type
 *
 * Used together with ENCLS leaves that add or modify an EPC page to an
 * enclave to define page permissions and type.
 */
struct sgx_secinfo {
	u64 flags;
	u8  reserved[SGX_SECINFO_RESERVED_SIZE];
} __packed __aligned(64);

#define SGX_PCMD_RESERVED_SIZE 40

/**
 * struct sgx_pcmd - Paging Crypto Metadata (PCMD)
 * @enclave_id:	enclave identifier
 * @mac:	MAC over PCMD, page contents and isvsvn
 *
 * PCMD is stored for every swapped page to the regular memory. When ELDU loads
 * the page back it recalculates the MAC by using a isvsvn number stored in a
 * VA page. Together these two structures bring integrity and rollback
 * protection.
 */
struct sgx_pcmd {
	struct sgx_secinfo secinfo;
	u64 enclave_id;
	u8  reserved[SGX_PCMD_RESERVED_SIZE];
	u8  mac[16];
} __packed __aligned(128);

#define SGX_SIGSTRUCT_RESERVED1_SIZE 84
#define SGX_SIGSTRUCT_RESERVED2_SIZE 20
#define SGX_SIGSTRUCT_RESERVED3_SIZE 32
#define SGX_SIGSTRUCT_RESERVED4_SIZE 12

/**
 * struct sgx_sigstruct_header -  defines author of the enclave
 * @header1:		constant byte string
 * @vendor:		must be either 0x0000 or 0x8086
 * @date:		YYYYMMDD in BCD
 * @header2:		costant byte string
 * @swdefined:		software defined value
 */
struct sgx_sigstruct_header {
	u64 header1[2];
	u32 vendor;
	u32 date;
	u64 header2[2];
	u32 swdefined;
	u8  reserved1[84];
} __packed;

/**
 * struct sgx_sigstruct_body - defines contents of the enclave
 * @miscselect:		additional information stored to an SSA frame
 * @misc_mask:		required miscselect in SECS
 * @attributes:		attributes for enclave
 * @xfrm:		XSave-Feature Request Mask (subset of XCR0)
 * @attributes_mask:	required attributes in SECS
 * @xfrm_mask:		required XFRM in SECS
 * @mrenclave:		SHA256-hash of the enclave contents
 * @isvprodid:		a user-defined value that is used in key derivation
 * @isvsvn:		a user-defined value that is used in key derivation
 */
struct sgx_sigstruct_body {
	u32 miscselect;
	u32 misc_mask;
	u8  reserved2[20];
	u64 attributes;
	u64 xfrm;
	u64 attributes_mask;
	u64 xfrm_mask;
	u8  mrenclave[32];
	u8  reserved3[32];
	u16 isvprodid;
	u16 isvsvn;
} __packed;

/**
 * struct sgx_sigstruct - an enclave signature
 * @header:		defines author of the enclave
 * @modulus:		the modulus of the public key
 * @exponent:		the exponent of the public key
 * @signature:		the signature calculated over the fields except modulus,
 * @body:		defines contents of the enclave
 * @q1:			a value used in RSA signature verification
 * @q2:			a value used in RSA signature verification
 *
 * Header and body are the parts that are actual signed. The remaining fields
 * define the signature of the enclave.
 */
struct sgx_sigstruct {
	struct sgx_sigstruct_header header;
	u8  modulus[SGX_MODULUS_SIZE];
	u32 exponent;
	u8  signature[SGX_MODULUS_SIZE];
	struct sgx_sigstruct_body body;
	u8  reserved4[12];
	u8  q1[SGX_MODULUS_SIZE];
	u8  q2[SGX_MODULUS_SIZE];
} __packed;

#define SGX_EINITTOKEN_RESERVED1_SIZE 11
#define SGX_EINITTOKEN_RESERVED2_SIZE 32
#define SGX_EINITTOKEN_RESERVED3_SIZE 32
#define SGX_EINITTOKEN_RESERVED4_SIZE 24

/**
 * struct sgx_einittoken - a token permitting to launch an enclave
 * @valid:			one if valid and zero if invalid
 * @attributes:			attributes for enclave
 * @xfrm:			XSave-Feature Request Mask (subset of XCR0)
 * @mrenclave:			SHA256-hash of the enclave contents
 * @mrsigner:			SHA256-hash of the public key used to sign the
 *				SIGSTRUCT
 * @le_cpusvn:			a value that reflects the SGX implementation
 *				running in in the CPU
 * @le_isvprodid:		a user-defined value that is used in key
 *				derivation
 * @le_isvsvn:			a user-defined value that is used in key
 *				derivation
 * @le_keyed_miscselect:	LE's miscselect masked with the token keys
 *				miscselect
 * @le_keyed_attributes:	LE's attributes masked with the token keys
 *				attributes
 * @le_keyed_xfrm:		LE's XFRM masked with the token keys xfrm
 * @salt:			random salt for wear-out protection
 * @mac:			CMAC over the preceding fields
 *
 * An enclave with EINITTOKENKEY attribute can access a key with the same name
 * by using ENCLS(EGETKEY) and use this to sign cryptographic tokens that can
 * be passed to ENCLS(EINIT) to permit the launch of other enclaves. This is
 * the only viable way to launch enclaves if IA32_SGXLEPUBKEYHASHn MSRs are
 * locked assuming that there is a Launch Enclave (LE) available that can be
 * used for generating these tokens.
 */
struct sgx_einittoken {
	u32 valid;
	u32 reserved1[SGX_EINITTOKEN_RESERVED1_SIZE];
	u64 attributes;
	u64 xfrm;
	u8  mrenclave[32];
	u8  reserved2[SGX_EINITTOKEN_RESERVED2_SIZE];
	u8  mrsigner[32];
	u8  reserved3[SGX_EINITTOKEN_RESERVED3_SIZE];
	u8  le_cpusvn[16];
	u16 le_isvprodid;
	u16 le_isvsvn;
	u8  reserved4[SGX_EINITTOKEN_RESERVED4_SIZE];
	u32 le_keyed_miscselect;
	u64 le_keyed_attributes;
	u64 le_keyed_xfrm;
	u8  salt[32];
	u8  mac[16];
} __packed __aligned(512);

#endif /* _ASM_X86_SGX_ARCH_H */
