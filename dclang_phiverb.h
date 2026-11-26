/* Generated from Aaron Krister Johnson's phiverb.lib. The UI and metadata
 * entry points were removed so this header can serve the interpreter and
 * standalone dclang2c-dsp output without a Faust architecture dependency.
 * ------------------------------------------------------------
name: "dclang-phiverb"
Code generated with Faust 2.92.3 (https://faust.grame.fr)
Compilation options: -lang c -fpga-mem-th 4 -ct 1 -cn dclang_phiverb -es 1 -mcd 16 -mdd 1024 -mdy 33 -single -ftz 0
------------------------------------------------------------ */

#ifndef  __dclang_phiverb_H__
#define  __dclang_phiverb_H__

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif


#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32)
#define RESTRICT __restrict
#else
#define RESTRICT __restrict__
#endif

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#ifndef FAUST_INT_WRAP
#define FAUST_INT_WRAP
static inline int faust_wrap_add(int a, int b) { return (int)((unsigned int)a + (unsigned int)b); }
static inline int faust_wrap_sub(int a, int b) { return (int)((unsigned int)a - (unsigned int)b); }
static inline int faust_wrap_mul(int a, int b) { return (int)((unsigned int)a * (unsigned int)b); }
#endif



#ifndef FAUSTCLASS
#define FAUSTCLASS dclang_phiverb
#endif

#ifdef __APPLE__
#define exp10f __exp10f
#define exp10 __exp10
#endif
#ifndef FAUSTMAXI
#define FAUSTMAXI
static inline int faustmaxi(int a, int b) { return (a > b) ? a : b; }
static inline int faustmini(int a, int b) { return (a < b) ? a : b; }
#endif

typedef struct
#if defined(__GNUC__) || defined(__clang__)
    __attribute__((may_alias))
#endif
{
	int iVec0[2];
	FAUSTFLOAT fHslider0;
	float fRec5[2];
	int fSampleRate;
	float fConst0;
	float fConst1;
	float fConst2;
	float fConst3;
	float fConst4;
	float fConst5;
	float fConst6;
	int iConst7;
	int iConst8;
	int iRec8[2];
	int iRec7[2];
	FAUSTFLOAT fHslider1;
	int IOTA0;
	float fVec1[8192];
	float fConst9;
	float fConst10;
	float fRec6[2];
	float fRec4[2];
	float fRec10[2];
	float fConst11;
	float fConst12;
	float fConst13;
	float fConst14;
	float fConst15;
	float fConst16;
	int iConst17;
	int iConst18;
	int iRec13[2];
	int iRec12[2];
	float fVec2[8192];
	float fConst19;
	float fConst20;
	float fRec11[2];
	float fRec9[2];
	float fRec15[2];
	float fConst21;
	float fConst22;
	float fConst23;
	float fConst24;
	float fConst25;
	float fConst26;
	int iConst27;
	int iConst28;
	int iRec18[2];
	int iRec17[2];
	float fVec3[8192];
	float fConst29;
	float fConst30;
	float fRec16[2];
	float fRec14[2];
	float fRec20[2];
	float fConst31;
	float fConst32;
	float fConst33;
	float fConst34;
	float fConst35;
	float fConst36;
	int iConst37;
	int iConst38;
	int iRec23[2];
	int iRec22[2];
	float fVec4[8192];
	float fConst39;
	float fConst40;
	float fRec21[2];
	float fRec19[2];
	float fRec25[2];
	float fConst41;
	float fConst42;
	float fConst43;
	float fConst44;
	float fConst45;
	int iConst46;
	int iConst47;
	int iRec28[2];
	int iRec27[2];
	float fVec5[8192];
	float fConst48;
	float fConst49;
	float fRec26[2];
	float fRec24[2];
	float fRec30[2];
	float fConst50;
	float fConst51;
	float fConst52;
	float fConst53;
	float fConst54;
	int iConst55;
	int iConst56;
	int iRec33[2];
	int iRec32[2];
	float fVec6[8192];
	float fConst57;
	float fConst58;
	float fRec31[2];
	float fRec29[2];
	float fRec35[2];
	float fConst59;
	float fConst60;
	float fConst61;
	float fConst62;
	float fConst63;
	int iConst64;
	int iConst65;
	int iRec38[2];
	int iRec37[2];
	float fVec7[8192];
	float fConst66;
	float fConst67;
	float fRec36[2];
	float fRec34[2];
	float fRec40[2];
	float fConst68;
	float fConst69;
	float fConst70;
	float fConst71;
	float fConst72;
	int iConst73;
	int iConst74;
	int iRec43[2];
	int iRec42[2];
	float fVec8[8192];
	float fConst75;
	float fConst76;
	float fRec41[2];
	float fRec39[2];
	float fVec9[2048];
	int iConst77;
	float fRec3[2];
	float fVec10[2048];
	int iConst78;
	float fRec2[2];
	float fVec11[2048];
	int iConst79;
	float fRec1[2];
	float fVec12[1024];
	int iConst80;
	float fRec0[2];
	float fVec13[2048];
	int iConst81;
	float fRec47[2];
	float fVec14[2048];
	int iConst82;
	float fRec46[2];
	float fVec15[2048];
	int iConst83;
	float fRec45[2];
	float fVec16[2048];
	int iConst84;
	float fRec44[2];
} dclang_phiverb;

static void classInitdclang_phiverb(int sample_rate) {
}

static void instanceResetUserInterfacedclang_phiverb(dclang_phiverb* dsp) {
	dsp->fHslider0 = (FAUSTFLOAT)(0.75f);
	dsp->fHslider1 = (FAUSTFLOAT)(0.7f);
}

static void instanceCleardclang_phiverb(dclang_phiverb* dsp) {
	/* C99 loop */
	{
		int l0;
		for (l0 = 0; l0 < 2; l0 = faust_wrap_add(l0, 1)) {
			dsp->iVec0[l0] = 0;
		}
	}
	/* C99 loop */
	{
		int l1;
		for (l1 = 0; l1 < 2; l1 = faust_wrap_add(l1, 1)) {
			dsp->fRec5[l1] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l2;
		for (l2 = 0; l2 < 2; l2 = faust_wrap_add(l2, 1)) {
			dsp->iRec8[l2] = 0;
		}
	}
	/* C99 loop */
	{
		int l3;
		for (l3 = 0; l3 < 2; l3 = faust_wrap_add(l3, 1)) {
			dsp->iRec7[l3] = 0;
		}
	}
	dsp->IOTA0 = 0;
	/* C99 loop */
	{
		int l4;
		for (l4 = 0; l4 < 8192; l4 = faust_wrap_add(l4, 1)) {
			dsp->fVec1[l4] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l5;
		for (l5 = 0; l5 < 2; l5 = faust_wrap_add(l5, 1)) {
			dsp->fRec6[l5] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l6;
		for (l6 = 0; l6 < 2; l6 = faust_wrap_add(l6, 1)) {
			dsp->fRec4[l6] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l7;
		for (l7 = 0; l7 < 2; l7 = faust_wrap_add(l7, 1)) {
			dsp->fRec10[l7] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l8;
		for (l8 = 0; l8 < 2; l8 = faust_wrap_add(l8, 1)) {
			dsp->iRec13[l8] = 0;
		}
	}
	/* C99 loop */
	{
		int l9;
		for (l9 = 0; l9 < 2; l9 = faust_wrap_add(l9, 1)) {
			dsp->iRec12[l9] = 0;
		}
	}
	/* C99 loop */
	{
		int l10;
		for (l10 = 0; l10 < 8192; l10 = faust_wrap_add(l10, 1)) {
			dsp->fVec2[l10] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l11;
		for (l11 = 0; l11 < 2; l11 = faust_wrap_add(l11, 1)) {
			dsp->fRec11[l11] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l12;
		for (l12 = 0; l12 < 2; l12 = faust_wrap_add(l12, 1)) {
			dsp->fRec9[l12] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l13;
		for (l13 = 0; l13 < 2; l13 = faust_wrap_add(l13, 1)) {
			dsp->fRec15[l13] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l14;
		for (l14 = 0; l14 < 2; l14 = faust_wrap_add(l14, 1)) {
			dsp->iRec18[l14] = 0;
		}
	}
	/* C99 loop */
	{
		int l15;
		for (l15 = 0; l15 < 2; l15 = faust_wrap_add(l15, 1)) {
			dsp->iRec17[l15] = 0;
		}
	}
	/* C99 loop */
	{
		int l16;
		for (l16 = 0; l16 < 8192; l16 = faust_wrap_add(l16, 1)) {
			dsp->fVec3[l16] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l17;
		for (l17 = 0; l17 < 2; l17 = faust_wrap_add(l17, 1)) {
			dsp->fRec16[l17] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l18;
		for (l18 = 0; l18 < 2; l18 = faust_wrap_add(l18, 1)) {
			dsp->fRec14[l18] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l19;
		for (l19 = 0; l19 < 2; l19 = faust_wrap_add(l19, 1)) {
			dsp->fRec20[l19] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l20;
		for (l20 = 0; l20 < 2; l20 = faust_wrap_add(l20, 1)) {
			dsp->iRec23[l20] = 0;
		}
	}
	/* C99 loop */
	{
		int l21;
		for (l21 = 0; l21 < 2; l21 = faust_wrap_add(l21, 1)) {
			dsp->iRec22[l21] = 0;
		}
	}
	/* C99 loop */
	{
		int l22;
		for (l22 = 0; l22 < 8192; l22 = faust_wrap_add(l22, 1)) {
			dsp->fVec4[l22] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l23;
		for (l23 = 0; l23 < 2; l23 = faust_wrap_add(l23, 1)) {
			dsp->fRec21[l23] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l24;
		for (l24 = 0; l24 < 2; l24 = faust_wrap_add(l24, 1)) {
			dsp->fRec19[l24] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l25;
		for (l25 = 0; l25 < 2; l25 = faust_wrap_add(l25, 1)) {
			dsp->fRec25[l25] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l26;
		for (l26 = 0; l26 < 2; l26 = faust_wrap_add(l26, 1)) {
			dsp->iRec28[l26] = 0;
		}
	}
	/* C99 loop */
	{
		int l27;
		for (l27 = 0; l27 < 2; l27 = faust_wrap_add(l27, 1)) {
			dsp->iRec27[l27] = 0;
		}
	}
	/* C99 loop */
	{
		int l28;
		for (l28 = 0; l28 < 8192; l28 = faust_wrap_add(l28, 1)) {
			dsp->fVec5[l28] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l29;
		for (l29 = 0; l29 < 2; l29 = faust_wrap_add(l29, 1)) {
			dsp->fRec26[l29] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l30;
		for (l30 = 0; l30 < 2; l30 = faust_wrap_add(l30, 1)) {
			dsp->fRec24[l30] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l31;
		for (l31 = 0; l31 < 2; l31 = faust_wrap_add(l31, 1)) {
			dsp->fRec30[l31] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l32;
		for (l32 = 0; l32 < 2; l32 = faust_wrap_add(l32, 1)) {
			dsp->iRec33[l32] = 0;
		}
	}
	/* C99 loop */
	{
		int l33;
		for (l33 = 0; l33 < 2; l33 = faust_wrap_add(l33, 1)) {
			dsp->iRec32[l33] = 0;
		}
	}
	/* C99 loop */
	{
		int l34;
		for (l34 = 0; l34 < 8192; l34 = faust_wrap_add(l34, 1)) {
			dsp->fVec6[l34] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l35;
		for (l35 = 0; l35 < 2; l35 = faust_wrap_add(l35, 1)) {
			dsp->fRec31[l35] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l36;
		for (l36 = 0; l36 < 2; l36 = faust_wrap_add(l36, 1)) {
			dsp->fRec29[l36] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l37;
		for (l37 = 0; l37 < 2; l37 = faust_wrap_add(l37, 1)) {
			dsp->fRec35[l37] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l38;
		for (l38 = 0; l38 < 2; l38 = faust_wrap_add(l38, 1)) {
			dsp->iRec38[l38] = 0;
		}
	}
	/* C99 loop */
	{
		int l39;
		for (l39 = 0; l39 < 2; l39 = faust_wrap_add(l39, 1)) {
			dsp->iRec37[l39] = 0;
		}
	}
	/* C99 loop */
	{
		int l40;
		for (l40 = 0; l40 < 8192; l40 = faust_wrap_add(l40, 1)) {
			dsp->fVec7[l40] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l41;
		for (l41 = 0; l41 < 2; l41 = faust_wrap_add(l41, 1)) {
			dsp->fRec36[l41] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l42;
		for (l42 = 0; l42 < 2; l42 = faust_wrap_add(l42, 1)) {
			dsp->fRec34[l42] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l43;
		for (l43 = 0; l43 < 2; l43 = faust_wrap_add(l43, 1)) {
			dsp->fRec40[l43] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l44;
		for (l44 = 0; l44 < 2; l44 = faust_wrap_add(l44, 1)) {
			dsp->iRec43[l44] = 0;
		}
	}
	/* C99 loop */
	{
		int l45;
		for (l45 = 0; l45 < 2; l45 = faust_wrap_add(l45, 1)) {
			dsp->iRec42[l45] = 0;
		}
	}
	/* C99 loop */
	{
		int l46;
		for (l46 = 0; l46 < 8192; l46 = faust_wrap_add(l46, 1)) {
			dsp->fVec8[l46] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l47;
		for (l47 = 0; l47 < 2; l47 = faust_wrap_add(l47, 1)) {
			dsp->fRec41[l47] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l48;
		for (l48 = 0; l48 < 2; l48 = faust_wrap_add(l48, 1)) {
			dsp->fRec39[l48] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l49;
		for (l49 = 0; l49 < 2048; l49 = faust_wrap_add(l49, 1)) {
			dsp->fVec9[l49] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l50;
		for (l50 = 0; l50 < 2; l50 = faust_wrap_add(l50, 1)) {
			dsp->fRec3[l50] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l51;
		for (l51 = 0; l51 < 2048; l51 = faust_wrap_add(l51, 1)) {
			dsp->fVec10[l51] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l52;
		for (l52 = 0; l52 < 2; l52 = faust_wrap_add(l52, 1)) {
			dsp->fRec2[l52] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l53;
		for (l53 = 0; l53 < 2048; l53 = faust_wrap_add(l53, 1)) {
			dsp->fVec11[l53] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l54;
		for (l54 = 0; l54 < 2; l54 = faust_wrap_add(l54, 1)) {
			dsp->fRec1[l54] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l55;
		for (l55 = 0; l55 < 1024; l55 = faust_wrap_add(l55, 1)) {
			dsp->fVec12[l55] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l56;
		for (l56 = 0; l56 < 2; l56 = faust_wrap_add(l56, 1)) {
			dsp->fRec0[l56] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l57;
		for (l57 = 0; l57 < 2048; l57 = faust_wrap_add(l57, 1)) {
			dsp->fVec13[l57] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l58;
		for (l58 = 0; l58 < 2; l58 = faust_wrap_add(l58, 1)) {
			dsp->fRec47[l58] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l59;
		for (l59 = 0; l59 < 2048; l59 = faust_wrap_add(l59, 1)) {
			dsp->fVec14[l59] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l60;
		for (l60 = 0; l60 < 2; l60 = faust_wrap_add(l60, 1)) {
			dsp->fRec46[l60] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l61;
		for (l61 = 0; l61 < 2048; l61 = faust_wrap_add(l61, 1)) {
			dsp->fVec15[l61] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l62;
		for (l62 = 0; l62 < 2; l62 = faust_wrap_add(l62, 1)) {
			dsp->fRec45[l62] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l63;
		for (l63 = 0; l63 < 2048; l63 = faust_wrap_add(l63, 1)) {
			dsp->fVec16[l63] = 0.0f;
		}
	}
	/* C99 loop */
	{
		int l64;
		for (l64 = 0; l64 < 2; l64 = faust_wrap_add(l64, 1)) {
			dsp->fRec44[l64] = 0.0f;
		}
	}
}

static void instanceConstantsdclang_phiverb(dclang_phiverb* dsp, int sample_rate) {
	dsp->fSampleRate = sample_rate;
	dsp->fConst0 = fminf(1.92e+05f, fmaxf(5e+01f, (float)(dsp->fSampleRate)));
	dsp->fConst1 = (float)((int)(0.05439909f * dsp->fConst0));
	dsp->fConst2 = dsp->fConst1 + -3.0f;
	dsp->fConst3 = dsp->fConst1 + -1.499995f;
	dsp->fConst4 = 0.0001f * dsp->fConst0;
	dsp->fConst5 = 1.0737418e+09f * (3.221f / dsp->fConst0);
	dsp->fConst6 = floorf(dsp->fConst5);
	dsp->iConst7 = (int)(dsp->fConst6);
	dsp->iConst8 = (int)(16777216.0f * (dsp->fConst5 - dsp->fConst6));
	dsp->fConst9 = dsp->fConst1 + -2.0f;
	dsp->fConst10 = dsp->fConst1 + -1.0f;
	dsp->fConst11 = (float)((int)(0.051405896f * dsp->fConst0));
	dsp->fConst12 = dsp->fConst11 + -3.0f;
	dsp->fConst13 = dsp->fConst11 + -1.499995f;
	dsp->fConst14 = 0.00028333333f * dsp->fConst0;
	dsp->fConst15 = 1.0737418e+09f * (0.891f / dsp->fConst0);
	dsp->fConst16 = floorf(dsp->fConst15);
	dsp->iConst17 = (int)(dsp->fConst16);
	dsp->iConst18 = (int)(16777216.0f * (dsp->fConst15 - dsp->fConst16));
	dsp->fConst19 = dsp->fConst11 + -2.0f;
	dsp->fConst20 = dsp->fConst11 + -1.0f;
	dsp->fConst21 = (float)((int)(0.048321996f * dsp->fConst0));
	dsp->fConst22 = dsp->fConst21 + -3.0f;
	dsp->fConst23 = dsp->fConst21 + -1.499995f;
	dsp->fConst24 = 0.00018333334f * dsp->fConst0;
	dsp->fConst25 = 1.0737418e+09f * (1.897f / dsp->fConst0);
	dsp->fConst26 = floorf(dsp->fConst25);
	dsp->iConst27 = (int)(dsp->fConst26);
	dsp->iConst28 = (int)(16777216.0f * (dsp->fConst25 - dsp->fConst26));
	dsp->fConst29 = dsp->fConst21 + -2.0f;
	dsp->fConst30 = dsp->fConst21 + -1.0f;
	dsp->fConst31 = (float)((int)(0.046553288f * dsp->fConst0));
	dsp->fConst32 = dsp->fConst31 + -3.0f;
	dsp->fConst33 = dsp->fConst31 + -1.499995f;
	dsp->fConst34 = 0.00016666666f * dsp->fConst0;
	dsp->fConst35 = 1.0737418e+09f * (2.341f / dsp->fConst0);
	dsp->fConst36 = floorf(dsp->fConst35);
	dsp->iConst37 = (int)(dsp->fConst36);
	dsp->iConst38 = (int)(16777216.0f * (dsp->fConst35 - dsp->fConst36));
	dsp->fConst39 = dsp->fConst31 + -2.0f;
	dsp->fConst40 = dsp->fConst31 + -1.0f;
	dsp->fConst41 = (float)((int)(0.043378685f * dsp->fConst0));
	dsp->fConst42 = dsp->fConst41 + -3.0f;
	dsp->fConst43 = dsp->fConst41 + -1.499995f;
	dsp->fConst44 = 1.0737418e+09f * (3.973f / dsp->fConst0);
	dsp->fConst45 = floorf(dsp->fConst44);
	dsp->iConst46 = (int)(dsp->fConst45);
	dsp->iConst47 = (int)(16777216.0f * (dsp->fConst44 - dsp->fConst45));
	dsp->fConst48 = dsp->fConst41 + -2.0f;
	dsp->fConst49 = dsp->fConst41 + -1.0f;
	dsp->fConst50 = (float)((int)(0.038480725f * dsp->fConst0));
	dsp->fConst51 = dsp->fConst50 + -3.0f;
	dsp->fConst52 = dsp->fConst50 + -1.499995f;
	dsp->fConst53 = 1.0737418e+09f * (1.11f / dsp->fConst0);
	dsp->fConst54 = floorf(dsp->fConst53);
	dsp->iConst55 = (int)(dsp->fConst54);
	dsp->iConst56 = (int)(16777216.0f * (dsp->fConst53 - dsp->fConst54));
	dsp->fConst57 = dsp->fConst50 + -2.0f;
	dsp->fConst58 = dsp->fConst50 + -1.0f;
	dsp->fConst59 = (float)((int)(0.03553288f * dsp->fConst0));
	dsp->fConst60 = dsp->fConst59 + -3.0f;
	dsp->fConst61 = dsp->fConst59 + -1.499995f;
	dsp->fConst62 = 1.0737418e+09f * (3.5f / dsp->fConst0);
	dsp->fConst63 = floorf(dsp->fConst62);
	dsp->iConst64 = (int)(dsp->fConst63);
	dsp->iConst65 = (int)(16777216.0f * (dsp->fConst62 - dsp->fConst63));
	dsp->fConst66 = dsp->fConst59 + -2.0f;
	dsp->fConst67 = dsp->fConst59 + -1.0f;
	dsp->fConst68 = (float)((int)(0.033582766f * dsp->fConst0));
	dsp->fConst69 = dsp->fConst68 + -3.0f;
	dsp->fConst70 = dsp->fConst68 + -1.499995f;
	dsp->fConst71 = 1.0737418e+09f * (3.1f / dsp->fConst0);
	dsp->fConst72 = floorf(dsp->fConst71);
	dsp->iConst73 = (int)(dsp->fConst72);
	dsp->iConst74 = (int)(16777216.0f * (dsp->fConst71 - dsp->fConst72));
	dsp->fConst75 = dsp->fConst68 + -2.0f;
	dsp->fConst76 = dsp->fConst68 + -1.0f;
	dsp->iConst77 = faustmini(1024, faustmaxi(0, faust_wrap_add((int)(0.0115419505f * dsp->fConst0), -1)));
	dsp->iConst78 = faustmini(1024, faustmaxi(0, faust_wrap_add((int)(0.009773242f * dsp->fConst0), -1)));
	dsp->iConst79 = faustmini(1024, faustmaxi(0, faust_wrap_add((int)(0.0069614514f * dsp->fConst0), -1)));
	dsp->iConst80 = faustmini(1024, faustmaxi(0, faust_wrap_add((int)(0.0050566895f * dsp->fConst0), -1)));
	dsp->iConst81 = faustmini(1024, faustmaxi(0, faust_wrap_add((int)(0.012630385f * dsp->fConst0), -1)));
	dsp->iConst82 = faustmini(1024, faustmaxi(0, faust_wrap_add((int)(0.010453515f * dsp->fConst0), -1)));
	dsp->iConst83 = faustmini(1024, faustmaxi(0, faust_wrap_add((int)(0.008594104f * dsp->fConst0), -1)));
	dsp->iConst84 = faustmini(1024, faustmaxi(0, faust_wrap_add((int)(0.00569161f * dsp->fConst0), -1)));
}

static void instanceInitdclang_phiverb(dclang_phiverb* dsp, int sample_rate) {
	instanceConstantsdclang_phiverb(dsp, sample_rate);
	instanceResetUserInterfacedclang_phiverb(dsp);
	instanceCleardclang_phiverb(dsp);
}

static void initdclang_phiverb(dclang_phiverb* dsp, int sample_rate) {
	classInitdclang_phiverb(sample_rate);
	instanceInitdclang_phiverb(dsp, sample_rate);
}

static void computedclang_phiverb(dclang_phiverb* dsp, int count, FAUSTFLOAT** RESTRICT inputs, FAUSTFLOAT** RESTRICT outputs) {
	FAUSTFLOAT* input0 = inputs[0];
	FAUSTFLOAT* input1 = inputs[1];
	FAUSTFLOAT* output0 = outputs[0];
	FAUSTFLOAT* output1 = outputs[1];
	float fSlow0 = (float)(dsp->fHslider0);
	float fSlow1 = 1.0f - fSlow0;
	float fSlow2 = (float)(dsp->fHslider1);
	float fSlow3 = sqrtf(fSlow2);
	/* C99 loop */
	{
		int i0;
		for (i0 = 0; i0 < count; i0 = faust_wrap_add(i0, 1)) {
			dsp->iVec0[0] = 1;
			dsp->fRec5[0] = fSlow1 * dsp->fRec5[1] + fSlow0 * dsp->fRec6[1];
			int iTemp0 = faust_wrap_sub(1, dsp->iVec0[1]);
			int iTemp1 = faust_wrap_add(dsp->iConst8, dsp->iRec8[1]);
			dsp->iRec8[0] = ((iTemp0) ? 0 : iTemp1 & 16777215);
			dsp->iRec7[0] = ((iTemp0) ? 0 : (faust_wrap_add(dsp->iConst7, faust_wrap_add(dsp->iRec7[1], iTemp1 >> 24))) & 1073741823);
			float fTemp2 = dsp->fConst4 * (-1.0f + 2.0f * (1.0f - fabsf(-1.0f + 1.1920929e-07f * (float)(dsp->iRec7[0] >> 6))));
			float fTemp3 = dsp->fConst3 + fTemp2;
			float fTemp4 = floorf(fTemp3);
			float fTemp5 = 0.124f * ((float)(input0[i0]) + (float)(input1[i0]));
			float fTemp6 = fTemp5 + fSlow3 * dsp->fRec5[0];
			dsp->fVec1[dsp->IOTA0 & 8191] = fTemp6;
			int iTemp7 = (int)(fminf(4098.0f, fmaxf(-2.0f, fTemp3)));
			float fTemp8 = dsp->fConst9 - fTemp4 + fTemp2;
			float fTemp9 = dsp->fConst10 - fTemp4 + fTemp2;
			dsp->fRec6[0] = (dsp->fConst2 - fTemp4 + fTemp2) * (0.5f * dsp->fVec1[(faust_wrap_sub(dsp->IOTA0, faustmini(4098, faustmaxi(0, iTemp7)))) & 8191] * fTemp8 - fTemp9 * dsp->fVec1[(faust_wrap_sub(dsp->IOTA0, faustmini(4098, faustmaxi(0, faust_wrap_add(1, iTemp7))))) & 8191]) + 0.5f * fTemp9 * fTemp8 * dsp->fVec1[(faust_wrap_sub(dsp->IOTA0, faustmini(4098, faustmaxi(0, faust_wrap_add(2, iTemp7))))) & 8191];
			dsp->fRec4[0] = 0.995f * dsp->fRec4[1] + dsp->fRec6[0] - dsp->fRec6[1];
			dsp->fRec10[0] = fSlow1 * dsp->fRec10[1] + fSlow0 * dsp->fRec11[1];
			int iTemp10 = faust_wrap_add(dsp->iConst18, dsp->iRec13[1]);
			dsp->iRec13[0] = ((iTemp0) ? 0 : iTemp10 & 16777215);
			dsp->iRec12[0] = ((iTemp0) ? 0 : (faust_wrap_add(dsp->iConst17, faust_wrap_add(dsp->iRec12[1], iTemp10 >> 24))) & 1073741823);
			float fTemp11 = dsp->fConst14 * (-1.0f + 2.0f * (1.0f - fabsf(-1.0f + 1.1920929e-07f * (float)(dsp->iRec12[0] >> 6))));
			float fTemp12 = dsp->fConst13 + fTemp11;
			float fTemp13 = floorf(fTemp12);
			float fTemp14 = fTemp5 + fSlow3 * dsp->fRec10[0];
			dsp->fVec2[dsp->IOTA0 & 8191] = fTemp14;
			int iTemp15 = (int)(fminf(4098.0f, fmaxf(-2.0f, fTemp12)));
			float fTemp16 = dsp->fConst19 - fTemp13 + fTemp11;
			float fTemp17 = dsp->fConst20 - fTemp13 + fTemp11;
			dsp->fRec11[0] = (dsp->fConst12 - fTemp13 + fTemp11) * (0.5f * dsp->fVec2[(faust_wrap_sub(dsp->IOTA0, faustmini(4098, faustmaxi(0, iTemp15)))) & 8191] * fTemp16 - fTemp17 * dsp->fVec2[(faust_wrap_sub(dsp->IOTA0, faustmini(4098, faustmaxi(0, faust_wrap_add(1, iTemp15))))) & 8191]) + 0.5f * fTemp17 * fTemp16 * dsp->fVec2[(faust_wrap_sub(dsp->IOTA0, faustmini(4098, faustmaxi(0, faust_wrap_add(2, iTemp15))))) & 8191];
			dsp->fRec9[0] = 0.995f * dsp->fRec9[1] + dsp->fRec11[0] - dsp->fRec11[1];
			dsp->fRec15[0] = fSlow1 * dsp->fRec15[1] + fSlow0 * dsp->fRec16[1];
			int iTemp18 = faust_wrap_add(dsp->iConst28, dsp->iRec18[1]);
			dsp->iRec18[0] = ((iTemp0) ? 0 : iTemp18 & 16777215);
			dsp->iRec17[0] = ((iTemp0) ? 0 : (faust_wrap_add(dsp->iConst27, faust_wrap_add(dsp->iRec17[1], iTemp18 >> 24))) & 1073741823);
			float fTemp19 = dsp->fConst24 * (-1.0f + 2.0f * (1.0f - fabsf(-1.0f + 1.1920929e-07f * (float)(dsp->iRec17[0] >> 6))));
			float fTemp20 = dsp->fConst23 + fTemp19;
			float fTemp21 = floorf(fTemp20);
			float fTemp22 = fTemp5 + fSlow3 * dsp->fRec15[0];
			dsp->fVec3[dsp->IOTA0 & 8191] = fTemp22;
			int iTemp23 = (int)(fminf(4098.0f, fmaxf(-2.0f, fTemp20)));
			float fTemp24 = dsp->fConst29 - fTemp21 + fTemp19;
			float fTemp25 = dsp->fConst30 - fTemp21 + fTemp19;
			dsp->fRec16[0] = (dsp->fConst22 - fTemp21 + fTemp19) * (0.5f * dsp->fVec3[(faust_wrap_sub(dsp->IOTA0, faustmini(4098, faustmaxi(0, iTemp23)))) & 8191] * fTemp24 - fTemp25 * dsp->fVec3[(faust_wrap_sub(dsp->IOTA0, faustmini(4098, faustmaxi(0, faust_wrap_add(1, iTemp23))))) & 8191]) + 0.5f * fTemp25 * fTemp24 * dsp->fVec3[(faust_wrap_sub(dsp->IOTA0, faustmini(4098, faustmaxi(0, faust_wrap_add(2, iTemp23))))) & 8191];
			dsp->fRec14[0] = 0.995f * dsp->fRec14[1] + dsp->fRec16[0] - dsp->fRec16[1];
			dsp->fRec20[0] = fSlow1 * dsp->fRec20[1] + fSlow0 * dsp->fRec21[1];
			int iTemp26 = faust_wrap_add(dsp->iConst38, dsp->iRec23[1]);
			dsp->iRec23[0] = ((iTemp0) ? 0 : iTemp26 & 16777215);
			dsp->iRec22[0] = ((iTemp0) ? 0 : (faust_wrap_add(dsp->iConst37, faust_wrap_add(dsp->iRec22[1], iTemp26 >> 24))) & 1073741823);
			float fTemp27 = dsp->fConst34 * (-1.0f + 2.0f * (1.0f - fabsf(-1.0f + 1.1920929e-07f * (float)(dsp->iRec22[0] >> 6))));
			float fTemp28 = dsp->fConst33 + fTemp27;
			float fTemp29 = floorf(fTemp28);
			float fTemp30 = fTemp5 + fSlow3 * dsp->fRec20[0];
			dsp->fVec4[dsp->IOTA0 & 8191] = fTemp30;
			int iTemp31 = (int)(fminf(4098.0f, fmaxf(-2.0f, fTemp28)));
			float fTemp32 = dsp->fConst39 - fTemp29 + fTemp27;
			float fTemp33 = dsp->fConst40 - fTemp29 + fTemp27;
			dsp->fRec21[0] = (dsp->fConst32 - fTemp29 + fTemp27) * (0.5f * dsp->fVec4[(faust_wrap_sub(dsp->IOTA0, faustmini(4098, faustmaxi(0, iTemp31)))) & 8191] * fTemp32 - fTemp33 * dsp->fVec4[(faust_wrap_sub(dsp->IOTA0, faustmini(4098, faustmaxi(0, faust_wrap_add(1, iTemp31))))) & 8191]) + 0.5f * fTemp33 * fTemp32 * dsp->fVec4[(faust_wrap_sub(dsp->IOTA0, faustmini(4098, faustmaxi(0, faust_wrap_add(2, iTemp31))))) & 8191];
			dsp->fRec19[0] = 0.995f * dsp->fRec19[1] + dsp->fRec21[0] - dsp->fRec21[1];
			dsp->fRec25[0] = fSlow1 * dsp->fRec25[1] + fSlow0 * dsp->fRec26[1];
			int iTemp34 = faust_wrap_add(dsp->iConst47, dsp->iRec28[1]);
			dsp->iRec28[0] = ((iTemp0) ? 0 : iTemp34 & 16777215);
			dsp->iRec27[0] = ((iTemp0) ? 0 : (faust_wrap_add(dsp->iConst46, faust_wrap_add(dsp->iRec27[1], iTemp34 >> 24))) & 1073741823);
			float fTemp35 = dsp->fConst4 * (-1.0f + 2.0f * (1.0f - fabsf(-1.0f + 1.1920929e-07f * (float)(dsp->iRec27[0] >> 6))));
			float fTemp36 = dsp->fConst43 + fTemp35;
			float fTemp37 = floorf(fTemp36);
			float fTemp38 = fTemp5 + fSlow3 * dsp->fRec25[0];
			dsp->fVec5[dsp->IOTA0 & 8191] = fTemp38;
			int iTemp39 = (int)(fminf(4098.0f, fmaxf(-2.0f, fTemp36)));
			float fTemp40 = dsp->fConst48 - fTemp37 + fTemp35;
			float fTemp41 = dsp->fConst49 - fTemp37 + fTemp35;
			dsp->fRec26[0] = (dsp->fConst42 - fTemp37 + fTemp35) * (0.5f * dsp->fVec5[(faust_wrap_sub(dsp->IOTA0, faustmini(4098, faustmaxi(0, iTemp39)))) & 8191] * fTemp40 - fTemp41 * dsp->fVec5[(faust_wrap_sub(dsp->IOTA0, faustmini(4098, faustmaxi(0, faust_wrap_add(1, iTemp39))))) & 8191]) + 0.5f * fTemp41 * fTemp40 * dsp->fVec5[(faust_wrap_sub(dsp->IOTA0, faustmini(4098, faustmaxi(0, faust_wrap_add(2, iTemp39))))) & 8191];
			dsp->fRec24[0] = 0.995f * dsp->fRec24[1] + dsp->fRec26[0] - dsp->fRec26[1];
			dsp->fRec30[0] = fSlow1 * dsp->fRec30[1] + fSlow0 * dsp->fRec31[1];
			int iTemp42 = faust_wrap_add(dsp->iConst56, dsp->iRec33[1]);
			dsp->iRec33[0] = ((iTemp0) ? 0 : iTemp42 & 16777215);
			dsp->iRec32[0] = ((iTemp0) ? 0 : (faust_wrap_add(dsp->iConst55, faust_wrap_add(dsp->iRec32[1], iTemp42 >> 24))) & 1073741823);
			float fTemp43 = dsp->fConst14 * (-1.0f + 2.0f * (1.0f - fabsf(-1.0f + 1.1920929e-07f * (float)(dsp->iRec32[0] >> 6))));
			float fTemp44 = dsp->fConst52 + fTemp43;
			float fTemp45 = floorf(fTemp44);
			float fTemp46 = fTemp5 + fSlow3 * dsp->fRec30[0];
			dsp->fVec6[dsp->IOTA0 & 8191] = fTemp46;
			int iTemp47 = (int)(fminf(4098.0f, fmaxf(-2.0f, fTemp44)));
			float fTemp48 = dsp->fConst57 - fTemp45 + fTemp43;
			float fTemp49 = dsp->fConst58 - fTemp45 + fTemp43;
			dsp->fRec31[0] = (dsp->fConst51 - fTemp45 + fTemp43) * (0.5f * dsp->fVec6[(faust_wrap_sub(dsp->IOTA0, faustmini(4098, faustmaxi(0, iTemp47)))) & 8191] * fTemp48 - fTemp49 * dsp->fVec6[(faust_wrap_sub(dsp->IOTA0, faustmini(4098, faustmaxi(0, faust_wrap_add(1, iTemp47))))) & 8191]) + 0.5f * fTemp49 * fTemp48 * dsp->fVec6[(faust_wrap_sub(dsp->IOTA0, faustmini(4098, faustmaxi(0, faust_wrap_add(2, iTemp47))))) & 8191];
			dsp->fRec29[0] = 0.995f * dsp->fRec29[1] + dsp->fRec31[0] - dsp->fRec31[1];
			dsp->fRec35[0] = fSlow1 * dsp->fRec35[1] + fSlow0 * dsp->fRec36[1];
			int iTemp50 = faust_wrap_add(dsp->iConst65, dsp->iRec38[1]);
			dsp->iRec38[0] = ((iTemp0) ? 0 : iTemp50 & 16777215);
			dsp->iRec37[0] = ((iTemp0) ? 0 : (faust_wrap_add(dsp->iConst64, faust_wrap_add(dsp->iRec37[1], iTemp50 >> 24))) & 1073741823);
			float fTemp51 = dsp->fConst24 * (-1.0f + 2.0f * (1.0f - fabsf(-1.0f + 1.1920929e-07f * (float)(dsp->iRec37[0] >> 6))));
			float fTemp52 = dsp->fConst61 + fTemp51;
			float fTemp53 = floorf(fTemp52);
			float fTemp54 = fTemp5 + fSlow3 * dsp->fRec35[0];
			dsp->fVec7[dsp->IOTA0 & 8191] = fTemp54;
			int iTemp55 = (int)(fminf(4098.0f, fmaxf(-2.0f, fTemp52)));
			float fTemp56 = dsp->fConst66 - fTemp53 + fTemp51;
			float fTemp57 = dsp->fConst67 - fTemp53 + fTemp51;
			dsp->fRec36[0] = (dsp->fConst60 - fTemp53 + fTemp51) * (0.5f * dsp->fVec7[(faust_wrap_sub(dsp->IOTA0, faustmini(4098, faustmaxi(0, iTemp55)))) & 8191] * fTemp56 - fTemp57 * dsp->fVec7[(faust_wrap_sub(dsp->IOTA0, faustmini(4098, faustmaxi(0, faust_wrap_add(1, iTemp55))))) & 8191]) + 0.5f * fTemp57 * fTemp56 * dsp->fVec7[(faust_wrap_sub(dsp->IOTA0, faustmini(4098, faustmaxi(0, faust_wrap_add(2, iTemp55))))) & 8191];
			dsp->fRec34[0] = 0.995f * dsp->fRec34[1] + dsp->fRec36[0] - dsp->fRec36[1];
			dsp->fRec40[0] = fSlow1 * dsp->fRec40[1] + fSlow0 * dsp->fRec41[1];
			int iTemp58 = faust_wrap_add(dsp->iConst74, dsp->iRec43[1]);
			dsp->iRec43[0] = ((iTemp0) ? 0 : iTemp58 & 16777215);
			dsp->iRec42[0] = ((iTemp0) ? 0 : (faust_wrap_add(dsp->iConst73, faust_wrap_add(dsp->iRec42[1], iTemp58 >> 24))) & 1073741823);
			float fTemp59 = dsp->fConst34 * (-1.0f + 2.0f * (1.0f - fabsf(-1.0f + 1.1920929e-07f * (float)(dsp->iRec42[0] >> 6))));
			float fTemp60 = dsp->fConst70 + fTemp59;
			float fTemp61 = floorf(fTemp60);
			float fTemp62 = fTemp5 + fSlow3 * dsp->fRec40[0];
			dsp->fVec8[dsp->IOTA0 & 8191] = fTemp62;
			int iTemp63 = (int)(fminf(4098.0f, fmaxf(-2.0f, fTemp60)));
			float fTemp64 = dsp->fConst75 - fTemp61 + fTemp59;
			float fTemp65 = dsp->fConst76 - fTemp61 + fTemp59;
			dsp->fRec41[0] = (dsp->fConst69 - fTemp61 + fTemp59) * (0.5f * dsp->fVec8[(faust_wrap_sub(dsp->IOTA0, faustmini(4098, faustmaxi(0, iTemp63)))) & 8191] * fTemp64 - fTemp65 * dsp->fVec8[(faust_wrap_sub(dsp->IOTA0, faustmini(4098, faustmaxi(0, faust_wrap_add(1, iTemp63))))) & 8191]) + 0.5f * fTemp65 * fTemp64 * dsp->fVec8[(faust_wrap_sub(dsp->IOTA0, faustmini(4098, faustmaxi(0, faust_wrap_add(2, iTemp63))))) & 8191];
			dsp->fRec39[0] = 0.995f * dsp->fRec39[1] + dsp->fRec41[0] - dsp->fRec41[1];
			float fTemp66 = dsp->fRec4[0] + dsp->fRec9[0] + dsp->fRec14[0] + dsp->fRec19[0] + dsp->fRec24[0] + dsp->fRec29[0] + dsp->fRec34[0] + fSlow2 * dsp->fRec3[1] + dsp->fRec39[0];
			dsp->fVec9[dsp->IOTA0 & 2047] = fTemp66;
			dsp->fRec3[0] = dsp->fVec9[(faust_wrap_sub(dsp->IOTA0, dsp->iConst77)) & 2047];
			float fTemp67 = dsp->fRec3[1] + fSlow2 * (dsp->fRec2[1] - fTemp66);
			dsp->fVec10[dsp->IOTA0 & 2047] = fTemp67;
			dsp->fRec2[0] = dsp->fVec10[(faust_wrap_sub(dsp->IOTA0, dsp->iConst78)) & 2047];
			float fTemp68 = dsp->fRec2[1] + fSlow2 * (dsp->fRec1[1] - fTemp67);
			dsp->fVec11[dsp->IOTA0 & 2047] = fTemp68;
			dsp->fRec1[0] = dsp->fVec11[(faust_wrap_sub(dsp->IOTA0, dsp->iConst79)) & 2047];
			float fTemp69 = dsp->fRec1[1] + fSlow2 * (dsp->fRec0[1] - fTemp68);
			dsp->fVec12[dsp->IOTA0 & 1023] = fTemp69;
			dsp->fRec0[0] = dsp->fVec12[(faust_wrap_sub(dsp->IOTA0, dsp->iConst80)) & 1023];
			output0[i0] = (FAUSTFLOAT)(dsp->fRec0[1] - fSlow2 * fTemp69);
			float fTemp70 = dsp->fRec39[0] + dsp->fRec34[0] + dsp->fRec29[0] + dsp->fRec24[0] + dsp->fRec19[0] + dsp->fRec14[0] + dsp->fRec9[0] + dsp->fRec4[0] + fSlow2 * dsp->fRec47[1];
			dsp->fVec13[dsp->IOTA0 & 2047] = fTemp70;
			dsp->fRec47[0] = dsp->fVec13[(faust_wrap_sub(dsp->IOTA0, dsp->iConst81)) & 2047];
			float fTemp71 = dsp->fRec47[1] + fSlow2 * (dsp->fRec46[1] - fTemp70);
			dsp->fVec14[dsp->IOTA0 & 2047] = fTemp71;
			dsp->fRec46[0] = dsp->fVec14[(faust_wrap_sub(dsp->IOTA0, dsp->iConst82)) & 2047];
			float fTemp72 = dsp->fRec46[1] + fSlow2 * (dsp->fRec45[1] - fTemp71);
			dsp->fVec15[dsp->IOTA0 & 2047] = fTemp72;
			dsp->fRec45[0] = dsp->fVec15[(faust_wrap_sub(dsp->IOTA0, dsp->iConst83)) & 2047];
			float fTemp73 = dsp->fRec45[1] + fSlow2 * (dsp->fRec44[1] - fTemp72);
			dsp->fVec16[dsp->IOTA0 & 2047] = fTemp73;
			dsp->fRec44[0] = dsp->fVec16[(faust_wrap_sub(dsp->IOTA0, dsp->iConst84)) & 2047];
			output1[i0] = (FAUSTFLOAT)(dsp->fRec44[1] - fSlow2 * fTemp73);
			dsp->iVec0[1] = dsp->iVec0[0];
			dsp->fRec5[1] = dsp->fRec5[0];
			dsp->iRec8[1] = dsp->iRec8[0];
			dsp->iRec7[1] = dsp->iRec7[0];
			dsp->IOTA0 = faust_wrap_add(dsp->IOTA0, 1);
			dsp->fRec6[1] = dsp->fRec6[0];
			dsp->fRec4[1] = dsp->fRec4[0];
			dsp->fRec10[1] = dsp->fRec10[0];
			dsp->iRec13[1] = dsp->iRec13[0];
			dsp->iRec12[1] = dsp->iRec12[0];
			dsp->fRec11[1] = dsp->fRec11[0];
			dsp->fRec9[1] = dsp->fRec9[0];
			dsp->fRec15[1] = dsp->fRec15[0];
			dsp->iRec18[1] = dsp->iRec18[0];
			dsp->iRec17[1] = dsp->iRec17[0];
			dsp->fRec16[1] = dsp->fRec16[0];
			dsp->fRec14[1] = dsp->fRec14[0];
			dsp->fRec20[1] = dsp->fRec20[0];
			dsp->iRec23[1] = dsp->iRec23[0];
			dsp->iRec22[1] = dsp->iRec22[0];
			dsp->fRec21[1] = dsp->fRec21[0];
			dsp->fRec19[1] = dsp->fRec19[0];
			dsp->fRec25[1] = dsp->fRec25[0];
			dsp->iRec28[1] = dsp->iRec28[0];
			dsp->iRec27[1] = dsp->iRec27[0];
			dsp->fRec26[1] = dsp->fRec26[0];
			dsp->fRec24[1] = dsp->fRec24[0];
			dsp->fRec30[1] = dsp->fRec30[0];
			dsp->iRec33[1] = dsp->iRec33[0];
			dsp->iRec32[1] = dsp->iRec32[0];
			dsp->fRec31[1] = dsp->fRec31[0];
			dsp->fRec29[1] = dsp->fRec29[0];
			dsp->fRec35[1] = dsp->fRec35[0];
			dsp->iRec38[1] = dsp->iRec38[0];
			dsp->iRec37[1] = dsp->iRec37[0];
			dsp->fRec36[1] = dsp->fRec36[0];
			dsp->fRec34[1] = dsp->fRec34[0];
			dsp->fRec40[1] = dsp->fRec40[0];
			dsp->iRec43[1] = dsp->iRec43[0];
			dsp->iRec42[1] = dsp->iRec42[0];
			dsp->fRec41[1] = dsp->fRec41[0];
			dsp->fRec39[1] = dsp->fRec39[0];
			dsp->fRec3[1] = dsp->fRec3[0];
			dsp->fRec2[1] = dsp->fRec2[0];
			dsp->fRec1[1] = dsp->fRec1[0];
			dsp->fRec0[1] = dsp->fRec0[0];
			dsp->fRec47[1] = dsp->fRec47[0];
			dsp->fRec46[1] = dsp->fRec46[0];
			dsp->fRec45[1] = dsp->fRec45[0];
			dsp->fRec44[1] = dsp->fRec44[0];
		}
	}
}

#ifdef __cplusplus
}
#endif

#endif
