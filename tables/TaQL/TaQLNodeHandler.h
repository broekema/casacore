//# TaQLNodeHandler.h: Classes to handle the nodes in the raw TaQL parse tree
//# Copyright (C) 2005
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA

#ifndef TABLES_TAQLNODEHANDLER_H
#define TABLES_TAQLNODEHANDLER_H

//# Includes
#include <casacore/casa/aips.h>
#include <casacore/tables/TaQL/TaQLNodeVisitor.h>
#include <casacore/tables/TaQL/TaQLNodeDer.h>
#include <casacore/tables/TaQL/TableParseQuery.h>
#include <casacore/tables/TaQL/ExprNode.h>
#include <casacore/tables/TaQL/ExprNodeSet.h>
#include <casacore/casa/Containers/Record.h>
#include <casacore/casa/Containers/ValueHolder.h>
#include <vector>

namespace casacore { //# NAMESPACE CASACORE - BEGIN

//# Forward Declarations
class TaQLNodeHRValue;


// <summary>
// Class to handle the nodes in the raw TaQL parse tree.
// </summary>

// <use visibility=local>

// <reviewed reviewer="" date="" tests="tTableGram">
// </reviewed>

// <prerequisite>
//# Classes you should understand before using this one.
//   <li> <linkto class=TaQLNode>TaQLNode</linkto>
//   <li> Note 199 describing
//        <a href="../notes/199.html">
//        TaQL</a>
// </prerequisite>

// <synopsis> 
// TaQLNodeHandler is a specialization of class
// <linkto class=TaQLNodeVisitor>TaQLNodeVisitor</linkto>.
// It processes the raw TaQL parse tree generated by TableGram.
// The processing is done in a recursive way. It starts at the top
// (which is a SELECT, UPDATE, etc. expression) and the processing
// results of a query are stored in a TableParseQuery object.
// These objects are kept in a stack for possible nested queries.
// After a query is fully processed, it is executed. Usually the result
// is a table; only a CALC command gives a TableExprNode as result.
// </synopsis> 

// <motivation>
// Separating the raw query parsing from the query processing has
// several advantages compared to the old situation where parsing
// and processing were combined.
// <ul>
//  <li> The full command is parsed before any processing is done.
//       So in case of a parse error, no possibly expensive processing
//       has been done yet.
//  <li> In the future query optimization can be done in an easier way.
//  <li> Nested parsing is not possible. In case a Table is opened
//       with a virtual TaQL column, the parsing of that TaQL string
//       does not interfere with parsing the TaQL command.
//  <li> It is possible to use expressions in the column list.
//       That could not be done before, because the column list was
//       parsed/processed before the table list.
// </ul>
// </motivation>

class TaQLNodeHandler : public TaQLNodeVisitor
{
public:
  virtual ~TaQLNodeHandler();

  // Handle and process the raw parse tree.
  // The result contains a Table or TableExprNode object.
  TaQLNodeResult handleTree (const TaQLNode& tree,
                             const std::vector<const Table*>&);

  // Define the functions to visit each node type.
  // <group>
  virtual TaQLNodeResult visitConstNode    (const TaQLConstNodeRep& node);
  virtual TaQLNodeResult visitRegexNode    (const TaQLRegexNodeRep& node);
  virtual TaQLNodeResult visitUnaryNode    (const TaQLUnaryNodeRep& node);
  virtual TaQLNodeResult visitBinaryNode   (const TaQLBinaryNodeRep& node);
  virtual TaQLNodeResult visitMultiNode    (const TaQLMultiNodeRep& node);
  virtual TaQLNodeResult visitFuncNode     (const TaQLFuncNodeRep& node);
  virtual TaQLNodeResult visitRangeNode    (const TaQLRangeNodeRep& node);
  virtual TaQLNodeResult visitIndexNode    (const TaQLIndexNodeRep& node);
  virtual TaQLNodeResult visitKeyColNode   (const TaQLKeyColNodeRep& node);
  virtual TaQLNodeResult visitTableNode    (const TaQLTableNodeRep& node);
  virtual TaQLNodeResult visitColNode      (const TaQLColNodeRep& node);
  virtual TaQLNodeResult visitColumnsNode  (const TaQLColumnsNodeRep& node);
  virtual TaQLNodeResult visitJoinNode     (const TaQLJoinNodeRep& node);
  virtual TaQLNodeResult visitGroupNode    (const TaQLGroupNodeRep& node);
  virtual TaQLNodeResult visitSortKeyNode  (const TaQLSortKeyNodeRep& node);
  virtual TaQLNodeResult visitSortNode     (const TaQLSortNodeRep& node);
  virtual TaQLNodeResult visitLimitOffNode (const TaQLLimitOffNodeRep& node);
  virtual TaQLNodeResult visitGivingNode   (const TaQLGivingNodeRep& node);
  virtual TaQLNodeResult visitUpdExprNode  (const TaQLUpdExprNodeRep& node);
  virtual TaQLNodeResult visitSelectNode   (const TaQLSelectNodeRep& node);
  virtual TaQLNodeResult visitUpdateNode   (const TaQLUpdateNodeRep& node);
  virtual TaQLNodeResult visitInsertNode   (const TaQLInsertNodeRep& node);
  virtual TaQLNodeResult visitDeleteNode   (const TaQLDeleteNodeRep& node);
  virtual TaQLNodeResult visitCountNode    (const TaQLCountNodeRep& node);
  virtual TaQLNodeResult visitCalcNode     (const TaQLCalcNodeRep& node);
  virtual TaQLNodeResult visitCreTabNode   (const TaQLCreTabNodeRep& node);
  virtual TaQLNodeResult visitColSpecNode  (const TaQLColSpecNodeRep& node);
  virtual TaQLNodeResult visitRecFldNode   (const TaQLRecFldNodeRep& node);
  virtual TaQLNodeResult visitUnitNode     (const TaQLUnitNodeRep& node);
  virtual TaQLNodeResult visitAltTabNode   (const TaQLAltTabNodeRep& node);
  virtual TaQLNodeResult visitAddColNode   (const TaQLAddColNodeRep& node);
  virtual TaQLNodeResult visitSetKeyNode   (const TaQLSetKeyNodeRep& node);
  virtual TaQLNodeResult visitRenDropNode  (const TaQLRenDropNodeRep& node);
  virtual TaQLNodeResult visitAddRowNode   (const TaQLAddRowNodeRep& node);
  virtual TaQLNodeResult visitConcTabNode  (const TaQLConcTabNodeRep& node);
  virtual TaQLNodeResult visitShowNode     (const TaQLShowNodeRep& node);
  virtual TaQLNodeResult visitCopyColNode  (const TaQLCopyColNodeRep& node);
  virtual TaQLNodeResult visitDropTabNode  (const TaQLDropTabNodeRep& node);
  // </group>

  // Get the actual result object from the result.
  static const TaQLNodeHRValue& getHR (const TaQLNodeResult&);

private:
  // Push a new TableParseQuery on the stack.
  TableParseQuery* pushStack (TableParseQuery::CommandType);

  // Get the top of the TableParseQuery stack.
  TableParseQuery* topStack() const;

  // Pop the top from the TableParseQuery stack.
  void popStack();

  // Clear the select stack.
  void clearStack();

  // Handle the select command.
  // Optionally the command is not executed (needed for the EXISTS operator).
  TaQLNodeResult handleSelect (const TaQLSelectNodeRep& node, Bool doExec);

  // Handle a table name or temptable number in the given node
  // and put it in the value result.
  void handleTableName (TaQLNodeHRValue* hrval, const TaQLNode& node);
  
  // Handle a MultiNode containing table info.
  void handleTables (const TaQLMultiNode&, Bool addToFromList=True);

  // Handle a MultiNoide containing joins.
  void handleJoins (const TaQLMultiNode& node);

  // Make a ConcatTable from a nested set of tables.
  Table makeConcatTable (const TaQLMultiNodeRep& node);

  // Handle the WHERE clause.
  void handleWhere (const TaQLNode&);

  // Handle the HAVING clause.
  void handleHaving (const TaQLNode&);

  // Handle the UPDATE SET clause.
  void handleUpdate (const TaQLMultiNode&);

  // Handle the INSERT columns.
  void handleInsCol (const TaQLMultiNode&);

  // Handle the INSERT values.
  void handleInsVal (const TaQLNode&);

  // Handle the possible LIKE table DROP COLUMN part.
  void handleLikeDrop (const TaQLMultiNode& node);

  // Handle a column specification in a create table or add column.
  void handleColSpecs (const TaQLMultiNode&);

  // Handle a Multi RecFld representing a Record.
  Record handleMultiRecFld (const TaQLNode& node);


  //# Use vector instead of stack because it has random access
  //# (which is used in TableParse.cc).
  std::vector<TableParseQuery*> itsStack;
  //# The temporary tables referred to by $i in the TaQL string.
  std::vector<const Table*> itsTempTables;
};


// <summary>
// Class containing the result value of the handling of a TaQLNode.
// </summary>

// <use visibility=local>

// <reviewed reviewer="" date="" tests="tTableGram">
// </reviewed>

// <prerequisite>
//# Classes you should understand before using this one.
//   <li> <linkto class=TaQLNode>TaQLNodeResult</linkto>
//   <li> <linkto class=TaQLNode>TaQLNodeHandler</linkto>
//   <li> Note 199 describing
//        <a href="../notes/199.html">
//        TaQL</a>
// </prerequisite>

// <synopsis> 
// TaQLNodeHRValue is a specialization of class
// <linkto class=TaQLNodeResultRep>TaQLNodeResultRep</linkto>.
// It contains the values resulting from handling a particular node.
// The object is effectively a collection of all possible values that
// need to be returned. Which values are filled in, depends on which node
// has been processed.
// <note> The getHR function in TaQLNodeHandler is very useful to
// extract/cast the TaQLNodeHRValue object from the general
// TaQLNodeResult object.
// </note>
// </synopsis> 

class TaQLNodeHRValue: public TaQLNodeResultRep
{
public:
  TaQLNodeHRValue()
    : itsInt(-1), itsElem(0), itsSet(0), itsNames(0) {}
  TaQLNodeHRValue (const TableExprNode& expr)
    : itsInt(-1), itsExpr(expr), itsElem(0), itsSet(0), itsNames(0) {}
  virtual ~TaQLNodeHRValue();

  // Get the values.
  // <group>
  Int getInt() const
    { return itsInt; }
  const String& getString() const
    { return itsString; }
  const String& getAlias() const
    { return itsAlias; }
  const String& getNameMask() const
    { return itsNameMask; }
  const String& getDtype() const
    { return itsDtype; }
  const Record& getRecord() const
    { return itsRecord; }
  const ValueHolder& getValueHolder() const
    { return itsVH; }
  const Table& getTable() const
    { return itsTable; }
  const TableExprNode& getExpr() const
    { return itsExpr; }
  const TableExprNodeSetElem* getElem() const
    { return itsElem; }
  const TableExprNodeSet& getExprSet() const
    { return *itsSet; }
  const Vector<String>* getNames() const
    { return itsNames; }
  // </group>

  // Set the values.
  // If a pointer is given, it takes over the pointer.
  // <group>
  void setInt (Int ival)
    { itsInt = ival; }
  void setString (const String& str)
    { itsString = str; }
  void setAlias (const String& alias)
    { itsAlias = alias; }
  void setNameMask (const String& nameMask)
    { itsNameMask = nameMask; }
  void setDtype (const String& dtype)
    { itsDtype = dtype; }
  void setRecord (const Record& record)
    { itsRecord = record; }
  void setValueHolder (const ValueHolder& vh)
    { itsVH = vh; }
  void setTable (const Table& table)
    { itsTable = table; }
  void setExpr (const TableExprNode& expr)
    { itsExpr = expr; }
  void setElem (TableExprNodeSetElem* elem)
    { itsElem = elem; }
  void setExprSet (TableExprNodeSet* set)
    { itsSet = set; }
  void setNames (Vector<String>* names)
    { itsNames = names; }
  // </group>

private:
  Int    itsInt;
  String itsString;
  String itsAlias;
  String itsNameMask;
  String itsDtype;
  Record itsRecord;
  ValueHolder itsVH;
  Table  itsTable;
  TableExprNode         itsExpr;
  TableExprNodeSetElem* itsElem;
  TableExprNodeSet*     itsSet;
  Vector<String>*       itsNames;
};


//# This function can only be implemented after TaQLNodeHRBase is declared.
inline const TaQLNodeHRValue& TaQLNodeHandler::getHR (const TaQLNodeResult& res)
{
  return *(TaQLNodeHRValue*)(res.getRep());
}

  

} //# NAMESPACE CASACORE - END

#endif
