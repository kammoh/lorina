/* lorina: C++ parsing library
 * Copyright (C) 2018-2021  EPFL
 * Copyright (C) 2022  Heinz Riener
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

/*!
  \file ast_graph.hpp
  \brief Verilog AST Graph

  \author Heinz Riener
*/

#pragma once

#include <vector>
#include <unordered_map>

namespace lorina
{

class verilog_ast_visitor;
class verilog_ast_graph;

class ast_node;
class ast_numeral;
class ast_identifier;
class ast_arithmetic_identifier;
class ast_identifier_list;
class ast_array_select;
class ast_range_expression;
class ast_sign;
class ast_expression;
class ast_system_function;
class ast_input_declaration;
class ast_output_declaration;
class ast_wire_declaration;
class ast_module_instantiation;
class ast_assignment;
class ast_parameter_declaration;
class ast_module;

class verilog_ast_visitor
{
public:
  explicit verilog_ast_visitor( const verilog_ast_graph* ag )
    : ag_( ag )
  {
  }

  virtual void visit( const ast_node& node ) { (void)node; }
  virtual void visit( const ast_numeral& node ) { (void)node; }
  virtual void visit( const ast_identifier& node ) { (void)node; }
  virtual void visit( const ast_arithmetic_identifier& node ) { (void)node; }
  virtual void visit( const ast_identifier_list& node ) { (void)node; }
  virtual void visit( const ast_array_select& node ) { (void)node; }
  virtual void visit( const ast_range_expression& node ) { (void)node; }
  virtual void visit( const ast_sign& node ) { (void)node; }
  virtual void visit( const ast_expression& node ) { (void)node; }
  virtual void visit( const ast_system_function& node ) { (void)node; }
  virtual void visit( const ast_input_declaration& node ) { (void)node; }
  virtual void visit( const ast_output_declaration& node ) { (void)node; }
  virtual void visit( const ast_wire_declaration& node ) { (void)node; }
  virtual void visit( const ast_module_instantiation& node ) { (void)node; }
  virtual void visit( const ast_assignment& node ) { (void)node; }
  virtual void visit( const ast_module& node ) { (void)node; }
  virtual void visit( const ast_parameter_declaration& node ) { (void)node; }

protected:
  const verilog_ast_graph* ag_;
}; // verilog_ast_visitor

using ast_id = uint32_t;

class ast_node
{
public:
  explicit ast_node( ast_id id )
    : id_( id )
  {
  }

  explicit ast_node( ast_id id, const std::vector<ast_id>& children )
    : id_( id )
    , children_( children )
  {
  }

  virtual ~ast_node() = default;

  ast_id id() const
  {
    return id_;
  }

  virtual void accept( verilog_ast_visitor& v ) const
  {
    v.visit( *this );
  }

  bool is_leaf() const
  {
    return children_.size() == 0u;
  }

  template<typename Fn>
  void foreach_child( Fn&& fn )
  {
    for ( ast_id& child : children_ )
    {
      fn( child );
    }
  }

  template<typename Fn>
  void foreach_child( Fn&& fn ) const
  {
    for ( const ast_id& child : children_ )
    {
      fn( child );
    }
  }

protected:
  uint32_t id_;
  std::vector<ast_id> children_;
}; // ast_node

/* \brief Numeral (leaf)
 *
 * A numeral.
 *
 */
class ast_numeral : public ast_node
{
public:
  explicit ast_numeral( ast_id id, const std::string& value )
    : ast_node( id )
    , value_( value )
  {
  }

  inline std::string value() const
  {
    return value_;
  }

  virtual inline void accept(verilog_ast_visitor& v) const override
  {
    v.visit( *this );
  }

protected:
  const std::string value_;
}; // ast_numeral

/* \brief Identifier (leaf)
 *
 * An identifier.
 *
 */
class ast_identifier : public ast_node
{
public:
  explicit ast_identifier( ast_id id, const std::string& identifier )
    : ast_node( id )
    , identifier_( identifier )
  {
  }

  inline std::string identifier() const
  {
    return identifier_;
  }

  inline void accept(verilog_ast_visitor& v) const override
  {
    v.visit( *this );
  }

protected:
  std::string identifier_;
}; // ast_identifier

/* \brief Arithmetic identifier (leaf)
 *
 * An identifier in an arithmetic expression.
 *
 */
class ast_arithmetic_identifier : public ast_node
{
public:
  explicit ast_arithmetic_identifier( ast_id id, const std::string& identifier )
    : ast_node( id )
    , identifier_( identifier )
  {
  }

  inline std::string identifier() const
  {
    return identifier_;
  }

  inline void accept(verilog_ast_visitor& v) const override
  {
    v.visit( *this );
  }

protected:
  std::string identifier_;
}; // ast_arithmetic_identifier

/* \brief Identifier list (not a leaf)
 *
 * A list of identifiers of form
 *   IDENTIFIER `,` ... `,` IDENTIFIER
 *
 */
class ast_identifier_list : public ast_node
{
public:
  explicit ast_identifier_list( ast_id id, const std::vector<ast_id>& identifiers )
    : ast_node( id, identifiers )
  {
  }

  inline std::vector<ast_id> identifiers() const
  {
    return children_;
  }

  inline void accept(verilog_ast_visitor& v) const override
  {
    v.visit( *this );
  }
}; // ast_identifier_list

/* \brief Array select
 *
 * An identifier followed by an bit selector of form
 *   IDENTIFIER `[` NUMERAL `]`
 *
 */
class ast_array_select : public ast_node
{
public:
  explicit ast_array_select( ast_id id, ast_id array, ast_id index )
    : ast_node( id, { array, index } )
  {
  }

  inline ast_id array() const
  {
    return children_[0];
  }

  inline ast_id index() const
  {
    return children_[1];
  }

  virtual inline void accept(verilog_ast_visitor& v) const override
  {
    v.visit( *this );
  }
}; // ast_array_select

/* \brief Range expression
 *
 * A pair of a most-significant bit and a least-significant bit of form
 *   `[` MSB `:` LSB `]`
 *
 */
class ast_range_expression : public ast_node
{
public:
  explicit ast_range_expression( ast_id id, ast_id hi, ast_id lo )
    : ast_node( id, { hi, lo } )
  {
  }

  inline ast_id hi() const
  {
    return children_[0];
  }

  inline ast_id lo() const
  {
    return children_[1];
  }

  virtual inline void accept(verilog_ast_visitor& v) const override
  {
    v.visit( *this );
  }
};

enum class sign_kind
{
  SIGN_MINUS = 1,
};

/* \brief Sign
 *
 */
class ast_sign : public ast_node
{
public:
  explicit ast_sign( ast_id id, sign_kind kind, ast_id expr )
    : ast_node( id, { expr } )
    , kind_( kind )
  {}

  inline ast_id expr() const
  {
    return children_[0];
  }

  inline sign_kind kind() const
  {
    return kind_;
  }

  virtual inline void accept(verilog_ast_visitor& v) const override
  {
    v.visit( *this );
  }

protected:
  sign_kind kind_;
}; // ast_sign

enum class expr_kind
{
  EXPR_ADD = 1,
  EXPR_MUL = 2,
  EXPR_NOT = 3,
  EXPR_AND = 4,
  EXPR_OR = 5,
  EXPR_XOR = 6,
};

/* \brief Expression
 *
 */
class ast_expression : public ast_node
{
public:
  explicit ast_expression( ast_id id, expr_kind kind, ast_id left )
    : ast_node( id, { left } )
    , kind_( kind )
  {
  }

  explicit ast_expression( ast_id id, expr_kind kind, ast_id left, ast_id right )
    : ast_node( id, { left, right } )
    , kind_( kind )
  {
  }

  inline ast_id left() const
  {
    assert( children_.size() >= 1 );
    return children_[0];
  }

  inline ast_id right() const
  {
    assert( children_.size() >= 2 );
    return children_[1];
  }

  inline expr_kind kind() const
  {
    return kind_;
  }

  virtual inline void accept(verilog_ast_visitor& v) const override
  {
    v.visit( *this );
  }

protected:
  expr_kind kind_;
};

/* \brief System function
 *
 */
class ast_system_function : public ast_node
{
public:
  explicit ast_system_function( ast_id id, ast_id fun, const std::vector<ast_id>& args )
    : ast_node( id, args )
    , fun_( fun )
  {
  }

  inline std::vector<ast_id> args() const
  {
    return children_;
  }

  inline ast_id fun() const
  {
    return fun_;
  }

  virtual inline void accept(verilog_ast_visitor& v) const override
  {
    v.visit( *this );
  }

protected:
  const ast_id fun_;
};

/* \brief Input declaration
 *
 * An input declaration of form
 *   `input` ( `[` NUMERAL `:` NUMERAL `]` )? IDENTIFIER `,` ... `,` IDENTIFIER `;`
 *
 */
class ast_input_declaration : public ast_node
{
public:
  explicit ast_input_declaration( ast_id id, const std::vector<ast_id>& identifiers )
    : ast_node( id, identifiers )
    , word_level_( false )
  {
  }

  explicit ast_input_declaration( ast_id id, const std::vector<ast_id>& identifiers, ast_id hi, ast_id lo )
    : ast_node( id, identifiers )
    , word_level_( true )
    , hi_( hi )
    , lo_( lo )
  {
  }

  inline bool word_level() const
  {
    return word_level_;
  }

  inline bool bit_level() const
  {
    return !word_level_;
  }

  inline std::vector<ast_id> identifiers() const
  {
    return children_;
  }

  inline ast_id hi() const
  {
    assert( word_level_ );
    return hi_;
  }

  inline ast_id lo() const
  {
    assert( word_level_ );
    return lo_;
  }

  virtual inline void accept(verilog_ast_visitor& v) const override
  {
    v.visit( *this );
  }

protected:
  bool word_level_;
  ast_id hi_;
  ast_id lo_;
}; // ast_input_declaration

/* \brief Output declaration
 *
 * An output declaration of form
 *   `output` ( `[` NUMERAL `:` NUMERAL `]` )? IDENTIFIER `,` ... `,` IDENTIFIER `;`
 *
 */
class ast_output_declaration : public ast_node
{
public:
  explicit ast_output_declaration( ast_id id, const std::vector<ast_id>& identifiers )
    : ast_node( id, identifiers )
    , word_level_( false )
  {
  }

  explicit ast_output_declaration( ast_id id, const std::vector<ast_id>& identifiers, ast_id hi, ast_id lo )
    : ast_node( id, identifiers )
    , word_level_( true )
    , hi_( hi )
    , lo_( lo )
  {
  }

  inline bool word_level() const
  {
    return word_level_;
  }

  inline bool bit_level() const
  {
    return !word_level_;
  }

  inline std::vector<ast_id> identifiers() const
  {
    return children_;
  }

  inline ast_id hi() const
  {
    assert( word_level_ );
    return hi_;
  }

  inline ast_id lo() const
  {
    assert( word_level_ );
    return lo_;
  }

  virtual inline void accept(verilog_ast_visitor& v) const override
  {
    v.visit( *this );
  }

protected:
  bool word_level_;
  ast_id hi_;
  ast_id lo_;
}; // ast_output_declaration

/* \brief Wire declaration
 *
 * An wire declaration of form
 *   `wire` ( `[` NUMERAL `:` NUMERAL `]` )? IDENTIFIER `,` ... `,` IDENTIFIER `;`
 *
 */
class ast_wire_declaration : public ast_node
{
public:
  explicit ast_wire_declaration( ast_id id, const std::vector<ast_id>& identifiers )
    : ast_node( id, identifiers )
    , word_level_( false )
  {
  }

  explicit ast_wire_declaration( ast_id id, const std::vector<ast_id>& identifiers, ast_id hi, ast_id lo )
    : ast_node( id, identifiers )
    , word_level_( true )
    , hi_( hi )
    , lo_( lo )
  {
  }

  inline bool word_level() const
  {
    return word_level_;
  }

  inline bool bit_level() const
  {
    return !word_level_;
  }

  inline std::vector<ast_id> identifiers() const
  {
    return children_;
  }

  inline ast_id hi() const
  {
    assert( word_level_ );
    return hi_;
  }

  inline ast_id lo() const
  {
    assert( word_level_ );
    return lo_;
  }

  virtual inline void accept(verilog_ast_visitor& v) const override
  {
    v.visit( *this );
  }

protected:
  bool word_level_;
  ast_id hi_;
  ast_id lo_;
}; // ast_wire_declaration

/* \brief Module instantiation
 *
 * A module instantiation of form
 *   IDENTIFIER (ParameterAssignment)? IDENTIFIER `(` PortAssignment `)` `;`
 * with
 *   ParameterAssignment ::= `#` `(` ARITH_EXPR `,` ... `,` ARITH_EXPR `)`
 *   PortAssignment ::= `.` IDENTIFIER(SIGNAL_REFERENCE) `,` ... `,` `.` IDENTIFIER(SIGNAL_REFERENCE).
 */
class ast_module_instantiation : public ast_node
{
public:
  explicit ast_module_instantiation( ast_id id, ast_id module_name, ast_id instance_name,
                                     const std::vector<std::pair<ast_id, ast_id>>& port_assignment,
                                     const std::vector<ast_id>& parameters )
    : ast_node( id )
    , module_name_( module_name )
    , instance_name_( instance_name )
    , port_assignment_( port_assignment )
    , parameters_( parameters )
  {
  }

  ast_id module_name() const
  {
    return module_name_;
  }

  ast_id instance_name() const
  {
    return instance_name_;
  }

  std::vector<std::pair<ast_id, ast_id>> port_assignment() const
  {
    return port_assignment_;
  }

  std::vector<ast_id> parameters() const
  {
    return parameters_;
  }

  virtual inline void accept(verilog_ast_visitor& v) const override
  {
    v.visit( *this );
  }

protected:
  ast_id module_name_;
  ast_id instance_name_;
  std::vector<std::pair<ast_id, ast_id>> port_assignment_;
  std::vector<ast_id> parameters_;
}; // ast_module_instantiation

/* \brief Parameter declaration
 *
 */
class ast_parameter_declaration : public ast_node
{
public:
  explicit ast_parameter_declaration( ast_id id, ast_id identifier, ast_id expr )
    : ast_node( id, { identifier, expr } )
  {
  }

  ast_id identifier() const
  {
    return children_[0];
  }

  ast_id expr() const
  {
    return children_[1];
  }

  virtual inline void accept(verilog_ast_visitor& v) const override
  {
    v.visit( *this );
  }
}; // ast_parameter_declaration

/* \brief Assignment statement
 *
 */
class ast_assignment : public ast_node
{
public:
  explicit ast_assignment( ast_id id, ast_id signal, ast_id expr )
    : ast_node( id, { signal, expr } )
  {
  }

  ast_id signal() const
  {
    return children_[0];
  }

  ast_id expr() const
  {
    return children_[1];
  }

  virtual inline void accept(verilog_ast_visitor& v) const override
  {
    v.visit( *this );
  }
}; // ast_assignment

/* \brief Module
 *
 */
class ast_module : public ast_node
{
public:
  explicit ast_module( ast_id id, const std::string& module_name, const std::vector<ast_id>& args, const std::vector<ast_id>& decls )
    : ast_node( id )
    , module_name_( module_name )
    , args_( args )
    , decls_( decls )
  {
  }

  std::string module_name() const
  {
    return module_name_;
  }

  std::vector<ast_id> args() const
  {
    return args_;
  }

  std::vector<ast_id> decls() const
  {
    return decls_;
  }

  virtual inline void accept(verilog_ast_visitor& v) const override
  {
    v.visit( *this );
  }

protected:
  const std::string module_name_;
  std::vector<ast_id> args_;
  std::vector<ast_id> decls_;
}; // ast_module

class verilog_ast_graph
{
public:
  class ast_or_error
  {
  public:
    explicit ast_or_error( ast_id ast )
      : ast_( ast )
    {
      ast_ |= 0x80000000;
    }

    explicit ast_or_error()
      : ast_( 0 )
    {
    }

    inline ast_id ast() const
    {
      return ast_ & 0x7FFFFFFF;
    }

    inline operator bool() const
    {
      return valid();
    }

    inline bool valid() const
    {
      return ast_ & 0x80000000;
    }

  protected:
    ast_id ast_;
  };

public:
  explicit verilog_ast_graph() = default;

  virtual ~verilog_ast_graph()
  {
    cleanup();
  }

  ast_node* node_ptr( ast_id id ) const
  {
    return nodes_.at( id );
  }

  template<typename T>
  T* node_as_ptr( ast_id id ) const
  {
    return static_cast<T*>( node_ptr( id ) );
  }

  template<typename T>
  const T& node_as_ref( ast_id id ) const
  {
    return *static_cast<T*>( node_ptr( id ) );
  }

  inline ast_id create_numeral( const std::string& numeral )
  {
    return create_node<ast_numeral>( numeral );
  }

  inline ast_id create_identifier( const std::string& identifier )
  {
    auto it = identifier_hash_.find( identifier );
    if ( it == std::end( identifier_hash_ ) )
    {
      auto id = create_node<ast_identifier>( identifier );
      identifier_hash_.emplace( identifier, id );
      return id;
    }
    else
    {
      return it->second;
    }
  }

  inline ast_id create_arithmetic_identifier( const std::string& identifier )
  {
    auto it = arithmetic_identifier_hash_.find( identifier );
    if ( it == std::end( arithmetic_identifier_hash_ ) )
    {
      auto id = create_node<ast_arithmetic_identifier>( identifier );
      arithmetic_identifier_hash_.emplace( identifier, id );
      return id;
    }
    else
    {
      return it->second;
    }
  }

  inline ast_id create_identifier_list( const std::vector<ast_id>& identifier_list )
  {
    return create_node<ast_identifier_list>( identifier_list );
  }

  inline ast_id create_range_expression( ast_id hi, ast_id lo )
  {
    return create_node<ast_range_expression>( hi, lo );
  }

  inline ast_id create_array_select( ast_id array, ast_id index )
  {
    return create_node<ast_array_select>( array, index );
  }

  inline ast_id create_sum_expression( ast_id term, ast_id expr )
  {
    return create_node<ast_expression>( expr_kind::EXPR_ADD, term, expr );
  }

  inline ast_id create_negative_sign( ast_id expr )
  {
    return create_node<ast_sign>( sign_kind::SIGN_MINUS, expr );
  }

  inline ast_id create_mul_expression( ast_id term, ast_id expr )
  {
    return create_node<ast_expression>( expr_kind::EXPR_MUL, term, expr );
  }

  inline ast_id create_not_expression( ast_id expr )
  {
    return create_node<ast_expression>( expr_kind::EXPR_NOT, expr );
  }

  inline ast_id create_and_expression( ast_id term, ast_id expr )
  {
    return create_node<ast_expression>( expr_kind::EXPR_AND, term, expr );
  }

  inline ast_id create_or_expression( ast_id term, ast_id expr )
  {
    return create_node<ast_expression>( expr_kind::EXPR_OR, term, expr );
  }

  inline ast_id create_xor_expression( ast_id term, ast_id expr )
  {
    return create_node<ast_expression>( expr_kind::EXPR_XOR, term, expr );
  }

  inline ast_id create_system_function( ast_id fun, const std::vector<ast_id>& args )
  {
    return create_node<ast_system_function>( fun, args );
  }

  inline ast_id create_input_declaration( ast_id id )
  {
    assert( id < nodes_.size() );

    ast_node* node = nodes_[id];
    if ( const ast_identifier_list* list = dynamic_cast<ast_identifier_list*>( node ) )
    {
      return create_node<ast_input_declaration>( list->identifiers() );
    }
    else if ( const ast_identifier* single = dynamic_cast<ast_identifier*>( node ) )
    {
      (void)single;
      return create_node<ast_input_declaration, std::vector<ast_id>>( { id } );
    }
    else
    {
      assert( false && "unknown node type" );
      std::abort();
    }
  }

  inline ast_id create_input_declaration( ast_id id, ast_id rid )
  {
    assert( id < nodes_.size() );
    assert( rid < nodes_.size() );

    ast_node* node = nodes_[id];
    ast_range_expression* range = static_cast<ast_range_expression*>( nodes_[rid] );
    if ( ast_identifier_list* list = dynamic_cast<ast_identifier_list*>( node ) )
    {
      return create_node<ast_input_declaration, std::vector<ast_id>, ast_id, ast_id>( list->identifiers(), range->hi(), range->lo() );
    }
    else if ( ast_identifier* single = dynamic_cast<ast_identifier*>( node ) )
    {
      (void)single;
      return create_node<ast_input_declaration, std::vector<ast_id>, ast_id, ast_id>( { id }, range->hi(), range->lo() );
    }
    else
    {
      assert( false && "unknown node type" );
      std::abort();
    }
  }

  inline ast_id create_output_declaration( ast_id id )
  {
    assert( id < nodes_.size() );

    ast_node* node = nodes_[id];
    if ( const ast_identifier_list* list = dynamic_cast<ast_identifier_list*>( node ) )
    {
      return create_node<ast_output_declaration>( list->identifiers() );
    }
    else if ( const ast_identifier* single = dynamic_cast<ast_identifier*>( node ) )
    {
      (void)single;
      return create_node<ast_output_declaration, std::vector<ast_id>>( { id } );
    }
    else
    {
      assert( false && "unknown node type" );
      std::abort();
    }
  }

  inline ast_id create_output_declaration( ast_id id, ast_id rid )
  {
    assert( id < nodes_.size() );
    assert( rid < nodes_.size() );

    ast_node* node = nodes_[id];
    ast_range_expression* range = static_cast<ast_range_expression*>( nodes_[rid] );
    if ( ast_identifier_list* list = dynamic_cast<ast_identifier_list*>( node ) )
    {
      return create_node<ast_output_declaration, std::vector<ast_id>, ast_id, ast_id>( list->identifiers(), range->hi(), range->lo() );
    }
    else if ( ast_identifier* single = dynamic_cast<ast_identifier*>( node ) )
    {
      (void)single;
      return create_node<ast_output_declaration, std::vector<ast_id>, ast_id, ast_id>( { id }, range->hi(), range->lo() );
    }
    else
    {
      assert( false && "unknown node type" );
      std::abort();
    }
  }

  inline ast_id create_wire_declaration( ast_id id )
  {
    assert( id < nodes_.size() );

    ast_node* node = nodes_[id];
    if ( const ast_identifier_list* list = dynamic_cast<ast_identifier_list*>( node ) )
    {
      return create_node<ast_wire_declaration>( list->identifiers() );
    }
    else if ( const ast_identifier* single = dynamic_cast<ast_identifier*>( node ) )
    {
      (void)single;
      return create_node<ast_wire_declaration, std::vector<ast_id>>( { id } );
    }
    else
    {
      assert( false && "unknown node type" );
      std::abort();
    }
  }

  inline ast_id create_wire_declaration( ast_id id, ast_id rid )
  {
    assert( id < nodes_.size() );
    assert( rid < nodes_.size() );

    ast_node* node = nodes_[id];
    ast_range_expression* range = static_cast<ast_range_expression*>( nodes_[rid] );
    if ( ast_identifier_list* list = dynamic_cast<ast_identifier_list*>( node ) )
    {
      return create_node<ast_wire_declaration, std::vector<ast_id>, ast_id, ast_id>( list->identifiers(), range->hi(), range->lo() );
    }
    else if ( ast_identifier* single = dynamic_cast<ast_identifier*>( node ) )
    {
      (void)single;
      return create_node<ast_wire_declaration, std::vector<ast_id>, ast_id, ast_id>( { id }, range->hi(), range->lo() );
    }
    else
    {
      assert( false && "unknown node type" );
      std::abort();
    }
  }

  inline ast_id create_module_instantiation( ast_id module_name, ast_id instance_name,
                                             const std::vector<std::pair<ast_id, ast_id>>& port_assignment,
                                             const std::vector<ast_id>& parameters )
  {
    return create_node<ast_module_instantiation>( module_name, instance_name, port_assignment, parameters );
  }

  inline ast_id create_parameter_declaration( ast_id identifier, ast_id expr )
  {
    return create_node<ast_parameter_declaration>( identifier, expr );
  }

  inline ast_id create_assignment( ast_id signal, ast_id expr )
  {
    return create_node<ast_assignment>( signal, expr );
  }

  inline ast_id create_module( const std::string& module_name, const std::vector<ast_id>& args, const std::vector<ast_id>& decls )
  {
    return create_node<ast_module>( module_name, args, decls );
  }

  void print() const
  {
    std::cout << "#nodes = " << nodes_.size() << std::endl;

    uint32_t counter{0};
    for ( const auto& node : nodes_ )
    {
      fmt::print( "{} {}\n", counter++, static_cast<void*>(node) );
    }
  }

protected:
  template<typename T, typename... Args>
  inline ast_id create_node( Args... args )
  {
    const uint32_t index = nodes_.size();
    nodes_.emplace_back( new T( index, args... ) );
    return index;
  }

protected:
  inline void cleanup()
  {
    for ( auto& node : nodes_ )
    {
      delete node;
    }
  }

protected:
  std::vector<ast_node*> nodes_;

  /* share identifiers */
  std::unordered_map<std::string, ast_id> identifier_hash_;

  /* share identifiers */
  std::unordered_map<std::string, ast_id> arithmetic_identifier_hash_;
}; // verilog_ast_graph

} // namespace lorina
