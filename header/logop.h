/* 
LOGOP.H - Logical Operators
$Revision: 4 $
Columbia Optimizer Framework

  A Joint Research Project of Portland State University 
  and the Oregon Graduate Institute
  Directed by Leonard Shapiro and David Maier
  Supported by NSF Grants IRI-9610013 and IRI-9619977
*/

//Logical operators

#ifndef LOGOP_H
#define LOGOP_H

#include "op.h"

#define		GET_ID			1111		// log_op name id for hash or compare
#define		EQJOIN_ID		2222
#define		PROJECT_ID		3333
#define		SELECT_ID		4444
#define		RM_DUPLICATES_ID	5555
#define		AGG_LIST_ID		6666
#define		FUNC_OP_ID		7777
#define		DUMMY_ID		8888

class GetLogicalOperator;
class SelectLogicalOperator;
class ProjectLogicalOperator;
class EQJoinLogicalOperator;
class DummyLogicalOperator;
class RMDuplicatesLogicalOperator;
class AggregateListLogicalOperator;
class FunctionLogicalOperator;

/*
============================================================
Class GET - GET DATA FROM DISK
============================================================
The GET operator retrieves data from disk storage.  It has no inputs
since it is the leaf operator in logical expressions.  The data retrieved
is specified by the CollID and RangeVar.  If the query included
from EMP as E, then EMP (actually, its ID) is the CollID and
E is the RangeVar.
*/

//##ModelId=3B0C087301E5
class GetLogicalOperator : public LogicalOperator
{
	
public :
    // If the query includes FROM EMP e, then EMP is the collection
    // and e is the range variable.
	//##ModelId=3B0C087301F0
	GetLogicalOperator ( CString collection,  CString rangeVar);
	//##ModelId=3B0C087301FB
	GetLogicalOperator ( int collId);	//Range Variable defaults to Collection here
	//##ModelId=3B0C08730204
	GetLogicalOperator( GetLogicalOperator& Op );
	//##ModelId=3B0C0873020E
	Operator * Clone() { return new GetLogicalOperator(*this); };
	
	//##ModelId=3B0C08730217
	~GetLogicalOperator() { if (TraceOn && !ForGlobalEpsPruning) ClassStat[C_GET].Delete(); };
	
	//##ModelId=3B0C08730218
	LOG_PROP * FindLogProp (LOG_PROP ** input);
	
	//##ModelId=3B0C08730222
	inline int GetArity() {return(0);};
	//##ModelId=3B0C0873022B
    inline CString GetName() {return ("GET"); };
	//##ModelId=3B0C08730235
	inline int GetNameId() {return GET_ID; };
	//##ModelId=3B0C0873023F
	inline int GetCollection() {return CollId;};
	//##ModelId=3B0C08730240
    inline bool operator==(Operator * other) 
	{	return ( other->GetNameId() == GetNameId() &&
	((GetLogicalOperator*)other)->CollId == CollId && 
	((GetLogicalOperator*)other)->RangeVar == RangeVar) ;
	};
	
	//##ModelId=3B0C0873024A
    CString Dump();
	
	//since this operator has arguments
	//##ModelId=3B0C08730253
    ub4 hash();
	
private :
	
	//##ModelId=3B0C0873025D
    int	CollId;
	//##ModelId=3B0C08730271
    CString RangeVar;
	
}; // GET

/*
   ============================================================
   EQJOIN
   ============================================================
   Natural Join, or Equijoin.  Since the predicates are so simple they
   are modeled as arguments.  If we ever get to manipulating general predicates,
   we may want to reconsider this decision.
*/

//##ModelId=3B0C087302DF
class EQJoinLogicalOperator : public LogicalOperator
{
	
public:
	
    // If the query includes
    //  	WHERE A.X = B.Y AND C.Z = D.W
    // then
    // lattrs is <A.X, C.Z> and
    //  rattrs is <B.Y, D.W>
	//##ModelId=3B0C087302F3
    int * lattrs;	// left attr's that are the same
	//##ModelId=3B0C087302FD
    int * rattrs;	// right attr's that are the same
	//##ModelId=3B0C08730307
	int size;		// number of the attrs
	
public :
	
	//##ModelId=3B0C08730311
	EQJoinLogicalOperator(int *lattrs, int *rattrs, int size);
	//##ModelId=3B0C08730326
	EQJoinLogicalOperator( EQJoinLogicalOperator& Op);	
	//##ModelId=3B0C08730328
	Operator * Clone() { return new EQJoinLogicalOperator(*this); };
	
	//##ModelId=3B0C08730330
	~EQJoinLogicalOperator() 
	{	
		if (TraceOn && !ForGlobalEpsPruning) ClassStat[C_EQJOIN].Delete();
		delete [] lattrs;
		delete [] rattrs;
	};
	
	//##ModelId=3B0C08730331
	LOG_PROP * FindLogProp (LOG_PROP ** input);
	
	//##ModelId=3B0C0873033B
	inline int GetArity() {return(2);};	//Inputs are left and right streams
	//##ModelId=3B0C08730344
    inline CString GetName() {return ("EQJOIN"); };   //Name of this operator
	//##ModelId=3B0C0873034E
	inline int GetNameId() {return EQJOIN_ID; };   //Name of this operator
	//##ModelId=3B0C08730358
	inline bool operator== (Operator * other)
	{
		return ( other->GetNameId() == GetNameId() &&
			    EqualArray( ((EQJoinLogicalOperator*)other)->lattrs,lattrs,size) &&  //arguments are equal
			    EqualArray( ((EQJoinLogicalOperator*)other)->rattrs,rattrs,size) );
	};
	
    //since this operator has arguments
	//##ModelId=3B0C08730362
    ub4 hash() ;
	
	//##ModelId=3B0C08730363
	CString Dump();
	
	// the following is used by Bill's Memory Manager
	// Redefine new and delete if memory manager is used.
#ifdef USE_MEMORY_MANAGER		// use bill's memory manager
	
public:									
	//##ModelId=3B0C08730377
	static BLOCK_ANCHOR * _anchor ;				
public:										
	// overload the new and delete methods
	//##ModelId=3B0C08730380
	inline void * operator new(size_t my_size)	
	{ return memory_manager -> allocate(&_anchor, (int) my_size); }
	
	//##ModelId=3B0C08730394
	inline void operator delete(void * dead_elem, size_t )	
	{ memory_manager -> deallocate(_anchor, dead_elem) ; }	
#endif
	
}; // EQJOIN

   /*
/*
   ============================================================
   DUMMY
   ============================================================
   For Paul Benninghoff's experiment.  Used to determine if optimizing two queries at
   once can result in significant savings.  DUMMY has two inputs, no transforms, one
   implementation, namely PDUMMY.
*/

//##ModelId=3B0C0874001A
class DummyLogicalOperator : public LogicalOperator
{
	
public :
	
	//##ModelId=3B0C0874002F
	DummyLogicalOperator();
	//##ModelId=3B0C08740030
	DummyLogicalOperator( DummyLogicalOperator& Op);	
	//##ModelId=3B0C08740039
	Operator * Clone() { return new DummyLogicalOperator(*this); };
	
	//##ModelId=3B0C08740042
	~DummyLogicalOperator() {};
	
	//##ModelId=3B0C08740043
	LOG_PROP * FindLogProp (LOG_PROP ** input);
	
	//##ModelId=3B0C0874004D
	inline int GetArity() {return(2);};	//Inputs are left and right streams
	//##ModelId=3B0C08740056
    inline CString GetName() {return ("DUMMY"); };   //Name of this operator
	//##ModelId=3B0C08740057
	inline int GetNameId() {return DUMMY_ID; };   //Name of this operator
	//##ModelId=3B0C08740060
	inline bool operator== (Operator * other) { return ( other->GetNameId() == GetNameId() );};
	
	//##ModelId=3B0C0874006B
    ub4 hash() ;
	
	//##ModelId=3B0C08740074
	CString Dump();
	
}; // DUMMY

/* ============================================================
   SELECT
   ============================================================
   This is the standard select operator: only input objects satisfying a predicate
   will be output.  However, we model the predicate of the operator as its second
   input.  Cascades modeled predicates as inputs in general, so that their structure
   could be handled with the Cascades transformation mechanism.  
*/

//##ModelId=3B0C087400F6
class SelectLogicalOperator : public LogicalOperator
{
	
public :
	
	//##ModelId=3B0C08740101
    SelectLogicalOperator();
	//##ModelId=3B0C0874010A
	SelectLogicalOperator( SelectLogicalOperator& Op);	
	//##ModelId=3B0C08740114
	Operator * Clone() { return new SelectLogicalOperator(*this); };
	
	//##ModelId=3B0C08740115
	~SelectLogicalOperator() { if (TraceOn && !ForGlobalEpsPruning) ClassStat[C_SELECT].Delete(); };
	
	//##ModelId=3B0C08740116
    LOG_PROP * FindLogProp (LOG_PROP ** input) ;
	
	//##ModelId=3B0C0874011F
	inline int GetArity() {return(2);};	// For input and predicate
	//##ModelId=3B0C08740128
    inline CString GetName() {return ("SELECT"); };   //Name of this operator
	//##ModelId=3B0C08740132
	inline int GetNameId() {return SELECT_ID; };   //Name of this operator
	//##ModelId=3B0C08740133
	inline bool operator== (Operator * other)
	{	return ( other->GetNameId() == GetNameId() ); }
	
	//##ModelId=3B0C0874013D
	ub4 hash();
	
	//##ModelId=3B0C08740146
	CString Dump();
}; // SELECT


/*
   ============================================================
   PROJECT
   ============================================================
   The standard project operator.  Attributes to project on are
   modeled as arguments.
*/

//##ModelId=3B0C087401DC
class ProjectLogicalOperator : public LogicalOperator
{
	
public :
	
	//##ModelId=3B0C087401F1
	int * attrs;	// attr's to project on
	//##ModelId=3B0C087401FB
	int size;		// the number of the attrs
	
	//##ModelId=3B0C08740205
    ProjectLogicalOperator(int * attrs, int size);
	//##ModelId=3B0C08740210
	ProjectLogicalOperator( ProjectLogicalOperator& Op);	
	//##ModelId=3B0C08740219
	Operator * Clone() { return new ProjectLogicalOperator(*this); };
	
	//##ModelId=3B0C0874021A
	~ProjectLogicalOperator() 
	{	
		if (TraceOn && !ForGlobalEpsPruning) ClassStat[C_PROJECT].Delete();
		delete [] attrs;
	};
	
	//##ModelId=3B0C08740223
    LOG_PROP * FindLogProp (LOG_PROP ** input);
	
	//##ModelId=3B0C0874022D
	inline int GetArity() {return(1);};	//Only input is the stream of bytes.  What to project is an argument, pattrs
	//##ModelId=3B0C0874022E
    inline CString GetName() {return ("PROJECT"); };   //Name of this operator
	//##ModelId=3B0C08740237
	inline int GetNameId() {return PROJECT_ID; };   //Name of this operator
	//##ModelId=3B0C08740241
	inline bool operator== (Operator * other)
	{
		return ( other->GetNameId() == GetNameId() &&
			   EqualArray( ((ProjectLogicalOperator*)other)->attrs, attrs, size )  ); //arguments are equal
	};
	
    //since this operator has arguments
	//##ModelId=3B0C0874024C
    ub4 hash();
	
	//##ModelId=3B0C0874024D
	CString Dump();
	
}; // PROJECT


/*
   ============================================================
   RM_DUPLICATES
   ============================================================
   standard remove duplicats operator.  The only input is the collections
   on which duplicates are removed
*/

//##ModelId=3B0C087402F5
class RMDuplicatesLogicalOperator : public LogicalOperator
{
	
public :
	
	//##ModelId=3B0C08740300
    RMDuplicatesLogicalOperator();  
	//##ModelId=3B0C08740309
	RMDuplicatesLogicalOperator( RMDuplicatesLogicalOperator& Op);	
	//##ModelId=3B0C0874030B
	Operator * Clone() { return new RMDuplicatesLogicalOperator(*this); };
	
	//##ModelId=3B0C08740313
	~RMDuplicatesLogicalOperator() 
	{ if (TraceOn && !ForGlobalEpsPruning) ClassStat[C_RM_DUPLICATES].Delete();	};
	
	//##ModelId=3B0C08740314
    LOG_PROP * FindLogProp (LOG_PROP ** input);
	
	//##ModelId=3B0C0874031E
	inline int GetArity() {return(1);};	//Only input is the stream of bytes. 
	//##ModelId=3B0C08740327
    inline CString GetName() {return ("RM_DUPLICATES"); };   //Name of this operator
	//##ModelId=3B0C08740328
	inline int GetNameId() {return RM_DUPLICATES_ID; };   //Name of this operator
	//##ModelId=3B0C08740331
	inline bool operator== (Operator * other)
	{
		return ( other->GetNameId() == GetNameId() ); 
	};
	
    //since this operator has arguments
	//##ModelId=3B0C0874033C
    ub4 hash();
	
	//##ModelId=3B0C08740345
	CString Dump();
	
}; // RM_DUPLICATES

/*
   ============================================================
   AGG_LIST
   ============================================================
   This operator combines aggregate functions (via AggOps),
	and the list of attributes grouped by (via GbyAtts).
	For example, 
		SELECT SUM(LE*(1-LD)) AS revenue
		FROM LineItem
		GROUP BY LD, LS
	becomes the operator
		AGG_LIST with parameters
			AggOps = <LE, LD> and GbyAtts <LE, LD>
	Note that AggOps and GbyAtts are arrays in general.  In this
	example each array has one element.
*/

//##ModelId=3B0C08750043
class AggregateListLogicalOperator : public LogicalOperator
{
public:
	//##ModelId=3B0C08750057
	int * GbyAtts;
	//##ModelId=3B0C08750061
	int GbySize;
	//##ModelId=3B0C08750076
	AGG_OP_ARRAY * AggOps;
	//##ModelId=3B0C0875007F
	int * FlattenedAtts;
	//##ModelId=3B0C08750093
	int FAttsSize;
	
	//##ModelId=3B0C0875009D
    AggregateListLogicalOperator(int * gby_atts, int gby_size, AGG_OP_ARRAY * agg_ops);  
	//##ModelId=3B0C087500B1
	AggregateListLogicalOperator( AggregateListLogicalOperator& Op)
		:GbyAtts(CopyArray(Op.GbyAtts, Op.GbySize)), GbySize(Op.GbySize),
		FAttsSize(Op.FAttsSize), FlattenedAtts(CopyArray(Op.FlattenedAtts, Op.FAttsSize))
	{	
		AggOps = new AGG_OP_ARRAY;
		AggOps->SetSize(Op.AggOps->GetSize());
		for (int i=0; i<Op.AggOps->GetSize(); i++)
		{
			(*AggOps)[i] = new AGG_OP( *(*Op.AggOps)[i]);
		}
		if (TraceOn && !ForGlobalEpsPruning) ClassStat[C_AGG_LIST].New();	
	};
	
	//##ModelId=3B0C087500BB
	Operator * Clone() { return new AggregateListLogicalOperator(*this); };
	
	//##ModelId=3B0C087500BC
	~AggregateListLogicalOperator() 
	{	
		delete [] GbyAtts;
		for (int i=0; i<AggOps->GetSize(); i++) delete (*AggOps)[i];
		delete AggOps;
		delete [] FlattenedAtts;
		if (TraceOn && !ForGlobalEpsPruning) ClassStat[C_AGG_LIST].Delete();	
	};
	
	//##ModelId=3B0C087500BD
    LOG_PROP * FindLogProp (LOG_PROP ** input);
	
	//##ModelId=3B0C087500C7
	inline int GetArity() {return(1);};	//Only input is the stream of bytes. 
	//##ModelId=3B0C087500D0
    inline CString GetName() {return ("AGG_LIST"); };   //Name of this operator
	//##ModelId=3B0C087500DA
	inline int GetNameId() {return AGG_LIST_ID; };   //Name of this operator
	//##ModelId=3B0C087500DB
	bool operator== (Operator * other);
	
    //since this operator has arguments
	//##ModelId=3B0C087500EE
    ub4 hash();
	
	//##ModelId=3B0C087500EF
	CString Dump();
	
}; // AGG_LIST

/*
   ============================================================
   FUNC_OP
   ============================================================
   FUNC_OP represents an operator which applies a function to the
   attributes in "attrs".  The function could be just arithmetic;
   if it involves an aggregate, see AGG_OP
*/

//##ModelId=3B0C087501CA
class FunctionLogicalOperator : public LogicalOperator
{
public:
	//##ModelId=3B0C087501DF
	CString RangeVar;
	//##ModelId=3B0C087501E8
	int * Atts;
	//##ModelId=3B0C087501F2
	int AttsSize;
	
	//##ModelId=3B0C087501FC
	FunctionLogicalOperator(CString range_var, int * atts, int size)
		: RangeVar(range_var), Atts(atts), AttsSize(size)
	{	if (TraceOn && !ForGlobalEpsPruning) ClassStat[C_FUNC_OP].New(); };
	
	//##ModelId=3B0C08750209
	FunctionLogicalOperator( FunctionLogicalOperator& Op )
		: RangeVar(Op.RangeVar), Atts( CopyArray(Op.Atts, Op.AttsSize) ), 
		AttsSize(Op.AttsSize)
	{	if (TraceOn && !ForGlobalEpsPruning) ClassStat[C_FUNC_OP].New();};
	
	//##ModelId=3B0C08750211
	Operator * Clone() {	return new FunctionLogicalOperator(*this); };
	
	//##ModelId=3B0C0875021A
	~FunctionLogicalOperator() 
	{	delete [] Atts;
	    if (TraceOn && !ForGlobalEpsPruning) ClassStat[C_FUNC_OP].Delete();
	};
	
	//##ModelId=3B0C08750224
	LOG_PROP * FindLogProp (LOG_PROP ** input);
	
	//##ModelId=3B0C08750226
	inline int GetArity() {return(1);};
	//##ModelId=3B0C0875022E
	inline CString GetName() {return ("FUNC_OP"); };
	//##ModelId=3B0C08750242
	inline int GetNameId() { return FUNC_OP_ID; };
	//##ModelId=3B0C0875024C
	inline bool operator== (Operator * other)
	{
		return ( other->GetNameId() == GetNameId() &&
			EqualArray( ((FunctionLogicalOperator*)other)->Atts, Atts, AttsSize) &&  //arguments are equal
			((FunctionLogicalOperator*)other)->RangeVar == RangeVar);
	};
	
	//since this operator has arguments
	//##ModelId=3B0C08750256
    ub4 hash();
	
	//##ModelId=3B0C08750257
	CString Dump();
};

#endif //LOGOP_H

