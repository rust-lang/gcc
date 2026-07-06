/* { dg-do compile } */
/* { dg-skip-if "-march given" { *-*-* } { "-march=*" } } */
/* { dg-options "-mcpu=spacemit-x60" { target { rv64 } } } */
/* Spacemit X60 => rva22u64_v_zicond_zifencei_zihintntl_zihpm_zfh_zbc_
 * zbkc_zvfh_zvfhmin_zvkt_zvl256b_smepmp_sscofpmf_sstc_svinval_svnapot_
 * svpbmt_xsmtvdot
 */

#if !((__riscv_xlen == 64)		\
      && !defined(__riscv_32e)		\
      && defined(__riscv_mul)		\
      && defined(__riscv_atomic)	\
      && (__riscv_flen == 64)		\
      && defined(__riscv_compressed)	\
      && defined(__riscv_v)		\
      && defined(__riscv_zic64b)	\
      && defined(__riscv_zba)		\
      && defined(__riscv_zbb)		\
      && defined(__riscv_zbc)		\
      && defined(__riscv_zbs)		\
      && defined(__riscv_zfh)		\
      && defined(__riscv_zfhmin)	\
      && defined(__riscv_ziccamoa)	\
      && defined(__riscv_ziccif)	\
      && defined(__riscv_zicclsm)	\
      && defined(__riscv_ziccrse)	\
      && defined(__riscv_zicsr)		\
      && defined(__riscv_zifencei)	\
      && defined(__riscv_zihintntl)	\
      && defined(__riscv_svpbmt)	\
      && defined(__riscv_smepmp)	\
      && defined(__riscv_sstc)		\
      && defined(__riscv_sscofpmf)	\
      && defined(__riscv_zicond)	\
      && defined(__riscv_zicboz)	\
      && defined(__riscv_zicbom)	\
      && defined(__riscv_zicbop)	\
      && defined(__riscv_zicntr)	\
      && defined(__riscv_zihpm)		\
      && defined(__riscv_za64rs)	\
      && defined(__riscv_zkt)		\
      && defined(__riscv_svinval)	\
      && defined(__riscv_svnapot)	\
      && defined(__riscv_zihintpause)	\
      && defined(__riscv_zbkc)		\
      && defined(__riscv_zvfh)		\
      && defined(__riscv_zvfhmin)	\
      && defined(__riscv_zvkt)		\
      && defined(__riscv_zvl256b)	\
      && defined(__riscv_xsmtvdot))
#error "unexpected arch"
#endif

int main()
{
  return 0;
}
