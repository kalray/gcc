#include <libmppahal/kv3/registers.h>
#include <stdio.h>

#define JTLB_NB_SET 64
#define JTLB_NB_WAYS 4
#define LTLB_NB_SET 1
#define LTLB_NB_WAYS 16

int
kvx_mmu_reset_tlb (int tlb_type, int nsets, int nways)
{
  int i, j;
  uint64_t mmc;

  mmc = KVX_SFR_MMC_SB_MASK * tlb_type;
  __builtin_kvx_set (KVX_SFR_MMC, mmc);
  __builtin_kvx_set (KVX_SFR_TEL, 0);

  for (i = 0; i < nsets; i++)
    {
      __builtin_kvx_set (KVX_SFR_TEH, KVX_SFR_SET_FIELD (0, TEH_PN, i));

      for (j = 0; j < nways; j++)
	{
	  __builtin_kvx_set (KVX_SFR_MMC, KVX_SFR_SET_FIELD (mmc, MMC_SW, j));
	  __builtin_kvx_tlbwrite ();

	  if (__builtin_kvx_get (KVX_SFR_MMC) & KVX_SFR_MMC_E_MASK)
	    {
	      __builtin_kvx_wfxl (KVX_SFR_MMC, KVX_SFR_MMC_E_WFXL_CLEAR);
	      fprintf (stderr, "Error: cannot init jtlb set %d way %d\n", i, j);
	      return 1;
	    }
	}
    }

  return 0;
}

int
init_mmu (void)
{
  uint64_t teh, tel, mmc;

  kvx_mmu_reset_tlb (0, JTLB_NB_SET, JTLB_NB_WAYS);
  kvx_mmu_reset_tlb (1, LTLB_NB_SET, LTLB_NB_WAYS);

  // map the first 512MB of DDR as uncached at 0xC0000000
  mmc = 0;
  __builtin_kvx_set (KVX_SFR_MMC, KVX_SFR_SET_FIELD (mmc, MMC_SW, 2));
  teh = KVX_SFR_SET_FIELD (0, TEH_PN, 0xC0000000 >> 12);
  teh = KVX_SFR_SET_FIELD (teh, TEH_ASN, 33); /* asn */
  teh = KVX_SFR_SET_FIELD (teh, TEH_G, 0);    /* global */
  teh = KVX_SFR_SET_FIELD (teh, TEL_PS, 3 /* 512MB */);
  __builtin_kvx_set (KVX_SFR_TEH, teh);

  tel = KVX_SFR_SET_FIELD (0, TEL_FN, 0x80000000 >> 12);
  tel = KVX_SFR_SET_FIELD (tel, TEL_PA, 4); /* PM: RWX, USER: NA */
  tel = KVX_SFR_SET_FIELD (tel, TEL_CP, 1); /* D: uncached, I: uncached */
  tel = KVX_SFR_SET_FIELD (tel, TEL_ES, 3); /* A-modified */
  __builtin_kvx_set (KVX_SFR_TEL, tel);

  __builtin_kvx_tlbwrite ();
  if (__builtin_kvx_get (KVX_SFR_MMC) & KVX_SFR_MMC_E_MASK)
    {
      __builtin_kvx_wfxl (KVX_SFR_MMC, KVX_SFR_MMC_E_WFXL_CLEAR);
      fprintf (stderr, "Error: cannot map the first 512MB of DDR\n");
      return 1;
    }

  // map the first 512MB of DDR as cached at its physical address
  mmc = KVX_SFR_MMC_SB_MASK;
  __builtin_kvx_set (KVX_SFR_MMC, KVX_SFR_SET_FIELD (mmc, MMC_SW, 4));
  teh = KVX_SFR_SET_FIELD (0, TEH_PN, 0x80000000 >> 12);
  teh = KVX_SFR_SET_FIELD (teh, TEH_ASN, 44); /* asn */
  teh = KVX_SFR_SET_FIELD (teh, TEH_G, 1);    /* global */
  teh = KVX_SFR_SET_FIELD (teh, TEL_PS, 3 /* 512MB */);
  __builtin_kvx_set (KVX_SFR_TEH, teh);

  tel = KVX_SFR_SET_FIELD (0, TEL_FN, 0x80000000 >> 12);
  tel = KVX_SFR_SET_FIELD (tel, TEL_PA, 4); /* PM: RWX, USER: NA */
  tel = KVX_SFR_SET_FIELD (tel, TEL_CP, 2); /* D: WT, I: cached */
  tel = KVX_SFR_SET_FIELD (tel, TEL_ES, 3); /* A-modified */
  __builtin_kvx_set (KVX_SFR_TEL, tel);

  __builtin_kvx_tlbwrite ();
  if (__builtin_kvx_get (KVX_SFR_MMC) & KVX_SFR_MMC_E_MASK)
    {
      __builtin_kvx_wfxl (KVX_SFR_MMC, KVX_SFR_MMC_E_WFXL_CLEAR);
      fprintf (stderr, "Error: cannot map the first 512MB of DDR\n");
      return 1;
    }

  return 0;
}

int
main (void)
{
  init_mmu ();

  return 0;
}
