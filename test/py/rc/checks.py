include("../xxx_helpers.py")

def doChecks():
	print("############################################")
	# Rules
	# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	# Let ABC = {A, B, C}
	# Let ABCe = {A, B, C, <no edge>}
	# We only enumerate connected rules
	# ==== ==== ================================== ============
	# L    R    Label variations                   Id Pattern
	# ==== ==== ================================== ============
	# (empty)                                      _
	# O         ABC                                l_
	#      O    ABC                                _l
	# O    O    ABC                                l_l
	# $    $    ABC x ABC                          l_l
	# ---- ---- ---------------------------------- ------------
	# O-O       ABC x ABC c ABC                    lel_
	# O-O' O    ABC x ABC x ABC                    lel_l
	# O-$  $    ABC x ABC x ABC x ABC              lel_l
	#      O-O  ABC x ABC x ABC                    _lel
	# O    O-O' ABC x ABC x ABC                    l_lel
	# $    O-$  ABC x ABC x ABC x ABC              l_lel
	# ---- ---- ---------------------------------- ------------
	# O-O' O-O' ABC x ABC x ABCe x ABC             lel_lel
	# O-$  O-$  ABC x ABC x ABC x ABCe x ABC       lel_lel
	# $-$' $-$' ABC x ABC x ABC x ABC x ABCe x ABC lel_lel
	# ==== ==== ================================== ============

	# ======= =======  =======  =========================
	# First   Second   New 
	# === === === ===  === ===  =========================
	# L   R   L   R    L   R    Note
	# === === === ===  === ===  =========================
	print("(empty) with everything"); checkNum = 1
	#         A        A
	#             A        A
	#         A   A    A   A
	#         A   B    A   B
	check(_ *rcParallel* A_, {iso(0, A_)})
	check(_ *rcParallel* _A, {iso(0, _A)})
	check(_ *rcParallel* A_A, {iso(0, A_A)})
	check(_ *rcParallel* A_B, {iso(0, A_B)})
	#         AAA      AAA
	#         AAA A    AAA A
	#             AAA      AAA
	#         A   AAA  A   AAA
	check(_ *rcParallel* AAA_, {iso(0, AAA_)})
	check(_ *rcParallel* AAA_A, {iso(0, AAA_A)})
	check(_ *rcParallel* _AAA, {iso(0, _AAA)})
	check(_ *rcParallel* A_AAA, {iso(0, A_AAA)})
	#         AAA A A  AAA A A
	#         A A AAA  A A AAA
	#         AAA AAA  AAA AAA
	#         AAA ABA  AAA ABA
	check(_ *rcParallel* AAA_AeA, {iso(0, AAA_AeA)})
	check(_ *rcParallel* AeA_AAA, {iso(0, AeA_AAA)})
	check(_ *rcParallel* AAA_AAA, {iso(0, AAA_AAA)})
	check(_ *rcParallel* AAA_ABA, {iso(0, AAA_ABA)})
	# ---------------------------------------------------
	# L   R   L   R    L   R    Note
	# === === === ===  === ===  =========================
	print("everything with (empty)"); checkNum = 1
	# A                A
	#     A                A
	# A   A            A   A
	# A   B            A   B
	# AAA A A          AAA A A
	# A A AAA          A A AAA
	# AAA AAA          AAA AAA
	# AAA ABA          AAA ABA
	check(A_ *rcParallel* _, {iso(0, A_)})
	check(_A *rcParallel* _, {iso(0, _A)})
	check(A_A *rcParallel* _, {iso(0, A_A)})
	check(A_B *rcParallel* _, {iso(0, A_B)})
	check(AAA_AeA *rcParallel* _, {iso(0, AAA_AeA)})
	check(AeA_AAA *rcParallel* _, {iso(0, AeA_AAA)})
	check(AAA_AAA *rcParallel* _, {iso(0, AAA_AAA)})
	check(AAA_ABA *rcParallel* _, {iso(0, AAA_ABA)})
	# ---------------------------------------------------
	# L   R   L   R    L   R    Note
	# === === === ===  === ===  =========================
	print("O -> with everything"); checkNum = 1
	# There would be no non-empty match
	# ---------------------------------------------------
	# L   R   L   R    L   R    Note
	# === === === ===  === ===  =========================
	print("-> O with everything"); checkNum = 1
	#     A   A                 Delete node
	#     A   A   A        A
	#     A   A   B        B
	#     A   AAB               Invalid, eSecond dangling in L
	#     A   A   AAB      AAB
	#     A   AAB A B           Invalid, eSecond dangling in L
	#     A   A B AAB    B AAB  (B_BAA)
	#     A   AAB AAB           Invalid, eSecond dangling in L
	#     A   AAB ABB           Invalid, eSecond dangling in L
	check(_A *rcCommon* A_, {iso(0, _)})
	check(_A *rcCommon* A_A, {iso(0, _A)})
	check(_A *rcCommon* A_B, {iso(0, _B)})
	check(_A *rcCommon* AAB_, {}, 0)
	check(_A *rcCommon* A_AAB, {iso(0, _AAB)})
	check(_A *rcCommon* AAB_AeB, {}, 0)
	check(_A *rcCommon* AeB_AAB, {iso(0, B_BAA)})
	check(_A *rcCommon* AAB_AAB, {}, 0)
	check(_A *rcCommon* AAB_ABB, {}, 0)
	# ---------------------------------------------------
	# L   R   L   R    L   R    Note
	# === === === ===  === ===  =========================
	print("O -> O with everything"); checkNum = 1
	# A   A   A        A
	# A   A   A   A    A   A
	# A   A   A   B    A   B
	# A   A   AAB      AAB
	# A   A   A   AAB  A   AAB
	# A   A   AAB A B  AAB A B
	# A   A   A B AAB  A B AAB
	# A   A   AAB AAB  AAB AAB
	# A   A   AAB ABB  AAB ABB
	check(A_A *rcCommon* A_, {iso(0, A_)})
	check(A_A *rcCommon* A_A, {iso(0, A_A)})
	check(A_A *rcCommon* A_B, {iso(0, A_B)})
	check(A_A *rcCommon* AAB_, {iso(0, AAB_)})
	check(A_A *rcCommon* A_AAB, {iso(0, A_AAB)})
	check(A_A *rcCommon* AAB_AeB, {iso(0, AAB_AeB)})
	check(A_A *rcCommon* AeB_AAB, {iso(0, AeB_AAB)})
	check(A_A *rcCommon* AAB_AAB, {iso(0, AAB_AAB)})
	check(A_A *rcCommon* AAB_ABB, {iso(0, AAB_ABB)})
	# ---------------------------------------------------
	# L   R   L   R    L   R    Note
	# === === === ===  === ===  =========================
	print("$ -> $ with everything"); checkNum = 1
	# A   B   B        A
	# A   B   B   B    A   B
	# A   B   B   A    A   A
	# A   B   B   C    A   C
	check(A_B *rcCommon* B_, {iso(0, A_)})
	check(A_B *rcCommon* B_B, {iso(0, A_B)})
	check(A_B *rcCommon* B_A, {iso(0, A_A)})
	check(A_B *rcCommon* B_C, {iso(0, A_C)})
	# ---------------------------------------------------
	# L   R   L   R    L   R    Note
	# === === === ===  === ===  =========================
	print("O-O -> with everything"); checkNum = 1
	# There would be no non-empty match
	# ---------------------------------------------------
	# L   R   L   R    L   R    Note
	# === === === ===  === ===  =========================
	print("O-O' -> O with everything"); checkNum = 1
	# This would be similar to just O -> O
	# ---------------------------------------------------
	# L   R   L   R    L   R    Note
	# === === === ===  === ===  =========================
	print("O-$ -> $ with everything"); checkNum = 1
	# This would be similar to just $ -> $
	# ---------------------------------------------------
	# L   R   L   R    L   R    Note
	# === === === ===  === ===  =========================
	print("-> O-O with everything"); checkNum = 1
	#     A A AAA               Invalid, eSecond dangling in L
	#     A A A A AAA      AAA
	check(_AeA *rcSub* AAA_, {}, 0)
	check(_AeA *rcSub* AeA_AAA, {iso(0, _AAA)})
	#     AAA A                 Invalid, eFirst dangling in R
	#     AAA A   A        AAA 
	#     AAA A   B        BAA  (AAB)
	#     AAA A A               Invalid, eFirst dangling in R
	#     AAA AAA               (All deleted)
	#     AAA ABA               (Only one vertex would be matched) Invalid, both edges dangling
	check(_AAA *rcCommon* A_, {}, 0)
	check(_AAA *rcCommon* A_A, {iso(0, _AAA)})
	check(_AAA *rcCommon* A_B, {iso(0, _AAB)})
	check(_AAA *rcSuper* AeA_, {}, 0)
	check(_AAA *rcCommon* AAA_, {iso(0, _)})
	check(_AAA *rcCommon* ABA_, {}, 0)
	#     AAA A A A             Invalid, eFirst dangling in R
	#     AAA AAA A        A
	#     AAA ABA A             (Only one vertex would be matched) Invalid, eSecond dangling in L
	check(_AAA *rcSuper* AeA_A, {}, 0)
	check(_AAA *rcCommon* AAA_A, {iso(0, _A)})
	check(_AAA *rcCommon* ABA_A, {}, 0)
	#     AAA A A AAA           Invalid, duplicate edge in R
	#     AAA AAA A A      A A
	#     AAA AAA AAA      AAA
	#     AAA AAA ABA      ABA
	check(_AAA *rcSuper* AeA_AAA, {}, 0)
	check(_AAA *rcSuper* AAA_AeA, {iso(0, _AeA)})
	check(_AAA *rcSuper* AAA_AAA, {iso(0, _AAA)})
	check(_AAA *rcSuper* AAA_ABA, {iso(0, _ABA)})
	# ---------------------------------------------------
	# L   R   L   R    L   R    Note
	# === === === ===  === ===  =========================
	print("O -> O-O with everything"); checkNum = 1
	# A   A A AAA               Invalid, eSecond dangling in L
	# A   A A AAA A             Invalid, eSecond dangling in L
	# A   A A A A AAA  A   AAA
	check(A_AeA *rcSub* AAA_, {}, 0)
	check(A_AeA *rcSub* AAA_A, {}, 0)
	check(A_AeA *rcSub* AeA_AAA, {iso(0, A_AAA)})
	# A   AAA A                 Invalid, eFirst dangling in R
	# A   AAA A   A    A   AAA 
	# A   AAA A   B    A   BAA
	#                  A   AAB  
	# A   AAA A A               Invalid, eFirst dangling in R
	# A   AAA AAA      A
	# A   AAA ABA               (Only one vertex would be matched) Invalid, both edges dangling
	check(A_AAA *rcCommon* A_, {}, 0)
	check(A_AAA *rcCommon* A_A, {iso(0, A_AAA)})
	check(A_AAA *rcCommon* A_B, {iso(0, A_AAB), iso(1, A_BAA)}, 2)
	check(A_AAA *rcSuper* AeA_, {}, 0)
	check(A_AAA *rcCommon* AAA_, {iso(0, A_)})
	check(A_AAA *rcCommon* ABA_, {}, 0)
	# A   AAA A A A             Invalid, eFirst dangling in R
	# A   AAA AAA A    A     A
	#                  A   A
	# A   AAA ABA A    ABA AAA' (Only one vertex would be matched, AB->A<- and AA->A<- are not the same)
	check(A_AAA *rcSuper* AeA_A, {}, 0)
	check(A_AAA *rcCommonMax* AAA_A, {iso(0, A__A), iso(1, A_A)}, 2)
	check(A_AAA *rcCommonMax* ABA_A, {iso(0, ABA__AAA)})
	# A   AAA A A AAA           Invalid, duplicate edge in R
	# A   AAA AAA A A  A   A A
	# A   AAA AAA AAA  A   AAA
	# A   AAA AAA ABA  A   ABA
	check(A_AAA *rcSuper* AeA_AAA, {}, 0)
	check(A_AAA *rcSuper* AAA_AeA, {iso(0, A_AeA)})
	check(A_AAA *rcSuper* AAA_AAA, {iso(0, A_AAA)})
	check(A_AAA *rcSuper* AAA_ABA, {iso(0, A_ABA)})
	# ---------------------------------------------------
	# L   R   L   R    L   R    Note
	# === === === ===  === ===  =========================
	print("O-O -> O-O with everything"); checkNum = 1
	# A A A A A A AAA  A A AAA
	# A A A A AAA A A  AAA A A
	# A A A A AAA A    AAA A  
	# A A A A AAA AAA  AAA AAA
	# A A A A AAA ABA  AAA ABA
	print("Group 1"); checkNum = 1
	check(AeA_AeA *rcSuper* AeA_AAA, {iso(0, AeA_AAA)})
	check(AeA_AeA *rcSub* AAA_AeA, {iso(0, AAA_AeA)})
	check(AeA_AeA *rcSub* AAA_A, {iso(0, AAA_A)})
	check(AeA_AeA *rcSub* AAA_AAA, {iso(0, AAA_AAA)})
	check(AeA_AeA *rcSub* AAA_ABA, {iso(0, AAA_ABA)})
	# A A AAA A                 Invalid, eFirst dangling in R
	# A A AAA A   A    A A AAA
	# A A AAA A A               Invalid, eFirst dangling in R
	# A A AAA AAA      A A
	# A A AAA A A A             Invalid, eFirst dangling in R
	# A A AAA AAA A    A A A
	print("Group 2"); checkNum = 1
	check(AeA_AAA *rcSuper* A_, {}, 0)
	check(AeA_AAA *rcSuper* A_A, {iso(0, AeA_AAA)})
	check(AeA_AAA *rcSuper* AeA_, {}, 0)
	check(AeA_AAA *rcSuper* AAA_, {iso(0, AeA_)})
	check(AeA_AAA *rcSuper* AeA_A, {}, 0)
	check(AeA_AAA *rcSuper* AAA_A, {iso(0, AeA_A)})
	# A A AAA A A A A  A A AAA
	# A A AAA A A AAA           Invalid, duplicate edge in R
	# A A AAA AAA A A  A A A A
	# A A AAA AAA AAA  A A AAA
	# A A AAA AAA ABA  A A ABA
	print("Group 3"); checkNum = 1
	check(AeA_AAA *rcSuper* AeA_AeA, {iso(0, AeA_AAA)})
	check(AeA_AAA *rcSuper* AeA_AAA, {}, 0)
	check(AeA_AAA *rcSuper* AAA_AeA, {iso(0, AeA_AeA)})
	check(AeA_AAA *rcSuper* AAA_AAA, {iso(0, AeA_AAA)})
	check(AeA_AAA *rcSuper* AAA_ABA, {iso(0, AeA_ABA)})
	# AAA A A A        AAA A
	# AAA A A A A A    AAA A
	# AAA A A AAA A             Invalid, duplicate edge in L
	print("Group 4"); checkNum = 1
	check(AAA_AeA *rcSuper* A_, {iso(0, AAA_A)})
	check(AAA_AeA *rcSuper* AeA_A, {iso(0, AAA_A)})
	check(AAA_AeA *rcSuper* AAA_A, {}, 0)
	# AAA A A A A A A  AAA A A
	# AAA A A A A AAA  AAA AAA
	# AAA A A A A ABA  AAA ABA
	# AAA A A AAA A A           Invalid, duplicate edge in L
	# AAA A A AAA AAA           Invalid, duplicate edge in L
	# AAA A A AAA ABA           Invalid, duplicate edge in L
	print("Group 5"); checkNum = 1
	check(AAA_AeA *rcSuper* AeA_AeA, {iso(0, AAA_AeA)})
	check(AAA_AeA *rcSuper* AeA_AAA, {iso(0, AAA_AAA)})
	check(AAA_AeA *rcSuper* AeA_ABA, {iso(0, AAA_ABA)})
	check(AAA_AeA *rcSub* AAA_AeA, {}, 0)
	check(AAA_AeA *rcSub* AAA_AAA, {}, 0)
	check(AAA_AeA *rcSub* AAA_ABA, {}, 0)
	# AAA AAA A                 Invalid, eFirst dangling in R
	# AAA AAA A   A    AAA AAA
	# AAA AAA A A A             Invalid, eFirst dangling in R
	# AAA AAA AAA A    AAA A
	print("Group 6"); checkNum = 1
	check(AAA_AAA *rcSuper* A_, {}, 0)
	check(AAA_AAA *rcSuper* A_A, {iso(0, AAA_AAA)})
	check(AAA_AAA *rcSuper* AeA_A, {}, 0)
	check(AAA_AAA *rcSuper* AAA_A, {iso(0, AAA_A)})
	# AAA AAA A A AAA           Invalid, duplicate edge in R
	# AAA AAA AAA A A  AAA A A
	# AAA AAA AAA AAA  AAA AAA
	# AAA AAA AAA ABA  AAA ABA
	print("Group 7"); checkNum = 1
	check(AAA_AAA *rcSuper* AeA_AAA, {}, 0)
	check(AAA_AAA *rcSuper* AAA_AeA, {iso(0, AAA_AeA)})
	check(AAA_AAA *rcSuper* AAA_AAA, {iso(0, AAA_AAA)})
	check(AAA_AAA *rcSuper* AAA_ABA, {iso(0, AAA_ABA)})
	# AAA ABA A A AAA           Invalid, duplicate edge in R
	# AAA ABA ABA A A  AAA A A
	# AAA ABA ABA ABA  AAA ABA
	# AAA ABA ABA AAA  AAA AAA
	# AAA ABA ABA ACA  AAA ACA
	print("Group 8"); checkNum = 1
	check(AAA_ABA *rcSuper* AeA_AAA, {}, 0)
	check(AAA_ABA *rcSuper* ABA_AeA, {iso(0, AAA_AeA)})
	check(AAA_ABA *rcSuper* ABA_ABA, {iso(0, AAA_ABA)})
	check(AAA_ABA *rcSuper* ABA_AAA, {iso(0, AAA_AAA)})
	check(AAA_ABA *rcSuper* ABA_ACA, {iso(0, AAA_ACA)})


	if False:
		# ===== ===== ===== ============== ===========
		# First Sec   New   Note
		# == == == == == == =================================================
		# L  R  L  R  L  R  
		# == == == == == == ==================================================

		# ===== ===== ===== ==================
		# First Sec   New   Note
		# == == == == == == =================================================
		# L  R  L  R  L  R  
		# == == == == == == ==================================================
		#    B  B           Del
		#    B  B  B     B
		#    B  B  A     A
		#    B  B  C     C
		check(_B *rcSuper* B_, {iso(0, _)})
		check(_B *rcSuper* B_B, {iso(0, _B)})
		check(_B *rcSuper* B_A, {iso(0, _A)})
		check(_B *rcSuper* B_C, {iso(0, _C)})
		# -- -- -- -- -- -- ------------------------------------------------
		# B  B  B     B
		# B  B  B  B  B  B
		# B  B  B  A  B  A
		# B  B  B  C  B  C
		check(B_B *rcSuper* B_, {iso(0, B_)})
		check(B_B *rcSuper* B_B, {iso(0, B_B)})
		check(B_B *rcSuper* B_A, {iso(0, B_A)})
		check(B_B *rcSuper* B_C, {iso(0, B_C)})
		# -- -- -- -- -- -- ------------------------------------------------
		# A  B  B     A
		# A  B  B  B  A  B
		# A  B  B  A  A  A
		# A  B  B  C  A  C
		check(A_B *rcSuper* B_, {iso(0, A_)})
		check(A_B *rcSuper* B_B, {iso(0, A_B)})
		check(A_B *rcSuper* B_A, {iso(0, A_A)})
		check(A_B *rcSuper* B_C, {iso(0, A_C)})
		# ===== ===== ===== ==================

