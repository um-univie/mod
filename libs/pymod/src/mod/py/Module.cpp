#include <boost/python/module.hpp>

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/comparison/equal.hpp>
#include <boost/preprocessor/if.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/tuple/size.hpp>

// BOOST_PP_SEQ(
//	BOOST_PP_TUPLE(Namespace,
//		BOOST_PP_SEQ(Name)
//	)
// )

#define MOD_FILES()                                                              \
   ((graph, (Printer))) /* this must be before DGGraphInterface due to default arg */ \
   ((Chem)) ((Collections)) ((Config)) ((Derivation))                            \
   ((dg, (Builder) (DG) (GraphInterface) (Printer) (Strategy)))                  \
   ((Error)) ((Function))                                                        \
   ((graph, (Automorphism) (Graph) (GraphInterface)))                            \
   ((rule, (RC) (Rule) (GraphInterface)))                                        \
   ((Misc)) ((Term))

namespace mod {
#define MOD_forwardDeclare(a) void BOOST_PP_CAT(a, _doExport)();
#define MOD_iterNs(r, data, i, t) MOD_forwardDeclare(t)
#define MOD_iterDecl(r, data, t)                                                 \
   BOOST_PP_IF(BOOST_PP_EQUAL(BOOST_PP_TUPLE_SIZE(t), 1),                        \
      namespace Py {                                                             \
         MOD_forwardDeclare(BOOST_PP_TUPLE_ELEM(2, 0, t))                        \
      },                                                                         \
      namespace BOOST_PP_TUPLE_ELEM(2, 0, t) {                                   \
      namespace Py {                                                             \
         BOOST_PP_SEQ_FOR_EACH_I(MOD_iterNs, ~, BOOST_PP_TUPLE_ELEM(2, 1, t))    \
      }}                                                                         \
   )
BOOST_PP_SEQ_FOR_EACH(MOD_iterDecl, ~, MOD_FILES())
#undef MOD_forwardDeclare
#undef MOD_iterNs
#undef MOD_iterDecl
} // namespace mod

BOOST_PYTHON_MODULE (libpymod) {
#define MOD_call(a, prefix) mod::prefix Py:: BOOST_PP_CAT(a, _doExport)();
#define MOD_iterNs(r, data, i, t) MOD_call(t, data::)
#define MOD_iterDecl(r, data, t)                                                \
   BOOST_PP_IF(BOOST_PP_EQUAL(BOOST_PP_TUPLE_SIZE(t), 1),                       \
      MOD_call(BOOST_PP_TUPLE_ELEM(2, 0, t),),                                  \
      BOOST_PP_SEQ_FOR_EACH_I(MOD_iterNs, BOOST_PP_TUPLE_ELEM(2, 0, t), BOOST_PP_TUPLE_ELEM(2, 1, t))      \
   )
	BOOST_PP_SEQ_FOR_EACH(MOD_iterDecl, ~, MOD_FILES())
#undef MOD_call
#undef MOD_iterNs
#undef MOD_iterDecl
}
