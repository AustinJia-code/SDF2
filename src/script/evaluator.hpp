/**
 * @file evaluator.hpp
 * @brief AST evaluator for the SDF2 scripting language.
 *
 * Each expression evaluates to a Value: a double, a FormPtr, or an axis vec3.
 * Built-in functions map directly to the existing C++ form factory functions.
 * 
 * Expects final form to be assigned to a variable named 'out'.
 */

#pragma once

#include "parser.hpp"
#include "core/sdf2.hpp"
#include <variant>
#include <unordered_map>
#include <stdexcept>
#include <string>

/**
 * Parse values as nums, forms, axes
 */
using Value = std::variant<double, FormPtr, gu::vec3_t>;

static double as_num (const Value& v, const std::string& ctx)
{
    if (auto* p = std::get_if<double> (&v)) return *p;
    throw std::runtime_error (ctx + ": expected a number");
}

static FormPtr as_form (const Value& v, const std::string& ctx)
{
    if (auto* p = std::get_if<FormPtr> (&v)) return *p;
    throw std::runtime_error (ctx + ": expected a form");
}

static gu::vec3_t as_axis (const Value& v, const std::string& ctx)
{
    if (auto* p = std::get_if<gu::vec3_t> (&v)) return *p;
    throw std::runtime_error (ctx + ": expected an axis (X, Y, or Z)");
}

/**
 * Eval
 */
class Evaluator
{
    std::unordered_map<std::string, Value> vars;

    Value eval (const Node* node)
    {
        if (auto* n = dynamic_cast<const NumberNode*> (node))
            return n->value;

        if (auto* n = dynamic_cast<const IDNode*> (node))
        {
            auto it = vars.find (n->name);
            if (it == vars.end ())
                throw std::runtime_error ("undefined variable '" + n->name + "'");

            return it->second;
        }

        if (auto* n = dynamic_cast<const BinOpNode*> (node))
        {
            double l = as_num (eval (n->left.get ()),  "arithmetic");
            double r = as_num (eval (n->right.get ()), "arithmetic");
            switch (n->op)
            {
                case '+': return l + r;
                case '-': return l - r;
                case '*': return l * r;
                case '/': return l / r;
            }
        }

        if (auto* n = dynamic_cast<const UnaryNode*> (node))
            return -as_num (eval (n->operand.get ()), "unary minus");

        if (auto* n = dynamic_cast<const CallNode*> (node))
            return eval_call (n);

        throw std::runtime_error ("unexpected expression");
    }

    std::vector<FormPtr> eval_form_list (const ListNode* node)
    {
        std::vector<FormPtr> forms;
        for (const auto& el : node->elements)
            forms.push_back (as_form (eval (el.get ()), "list element"));
        return forms;
    }

    Value eval_call (const CallNode* n)
    {
        const auto& f    = n->func;
        const auto& a    = n->args;
        size_t      argc = a.size ();

        auto arg  = [&] (size_t i)
                        { return eval (a[i].get ()); };

        auto num  = [&] (size_t i, const std::string& ctx)
                        { return as_num  (arg (i), ctx); };

        auto form = [&] (size_t i, const std::string& ctx)
                        { return as_form (arg (i), ctx); };

        auto axis = [&] (size_t i, const std::string& ctx)
                        { return as_axis (arg (i), ctx); };

        auto check = [&] (size_t lo, size_t hi)
        {
            if (argc < lo || argc > hi)
                throw std::runtime_error (
                    f + ": expected " + std::to_string (lo) +
                    (lo == hi ? "" : "-" + std::to_string (hi)) +
                    " arguments, got " + std::to_string (argc));
        };

        /**
         * Primitives
         */
        if (f == "sphere")
        {
            check (1, 1);
            return make_sphere (num (0, f));
        }
        if (f == "box")
        {
            check (3, 3);
            return make_box (num (0, f), num (1, f), num (2, f));
        }
        if (f == "cylinder")
        {
            check (2, 2);
            return make_cylinder (num (0, f), num (1, f));
        }
        if (f == "circprism")
        {
            check (3, 3);
            return make_circprism ((size_t) num (0, f), num (1, f), num (2, f));
        }
        if (f == "inscprism")
        {
            check (3, 3);
            return make_inscprism ((size_t) num (0, f), num (1, f), num (2, f));
        }

        /**
         * Booleans
         */
        if (f == "union")
        {
            if (argc < 1)
                throw std::runtime_error ("union: too few arguments");

            if (auto* lst = dynamic_cast<const ListNode*> (a[0].get ()))
            {
                double k = (argc >= 2) ? num (1, "union k") : 0.0;
                auto forms = eval_form_list (lst);
                if (forms.size () < 2)
                    throw std::runtime_error ("union: need at least 2 forms in list");
                    
                return build_union (forms, k);
            }
            check (2, 3);

            double k = (argc == 3)
                        ? num (2, "union k")
                        : 0.0;

            return build_union (form (0, "union"), form (1, "union"), k);
        }

        if (f == "intersection")
        {
            if (argc < 1)
                throw std::runtime_error ("intersection: too few arguments");

            if (auto* lst = dynamic_cast<const ListNode*> (a[0].get ()))
            {
                double k = (argc >= 2) ? num (1, "intersection k") : 0.0;
                auto forms = eval_form_list (lst);
                if (forms.size () < 2)
                    throw std::runtime_error ("intersection: need at least 2 forms in list");

                return build_intersection (forms, k);
            }

            check (2, 3);
            double k = (argc == 3)
                        ? num (2, "intersection k")
                        : 0.0;

            return build_intersection (form (0, "intersection"),
                                       form (1, "intersection"), k);
        }

        if (f == "difference")
        {
            check (2, 3);
            double k = (argc == 3)
                        ? num (2, "difference k")
                        : 0.0;

            return build_difference (form (0, "difference"),
                                     form (1, "difference"), k);
        }

        /**
         * Transforms
         */
        if (f == "translate")
        {
            check (4, 4);
            return add_translation (form (0, "translate"),
                                    {num (1, f), num (2, f), num (3, f)});
        }

        if (f == "rotate")
        {
            check (3, 3);
            return add_rotation (form (0, "rotate"),
                                 axis (1, "rotate"),
                                 num (2, "rotate"));
        }

        /**
         * Complex
         */
        if (f == "pattern")
        {
            check (4, 4);
            return build_pattern (form (0, "pattern"),
                                  (size_t) num (1, "pattern count"),
                                  axis (2, "pattern axis"),
                                  num (3, "pattern radius"));
        }

        throw std::runtime_error ("unknown function '" + f + "'");
    }

public:
    Evaluator ()
    {
        vars["X"] = X_AXIS;
        vars["Y"] = Y_AXIS;
        vars["Z"] = Z_AXIS;
    }

    void run (const Program& prog)
    {
        for (const auto& stmt : prog)
            vars[stmt.name] = eval (stmt.value.get ());
    }

    FormPtr result ()
    {
        auto it = vars.find ("out");
        if (it == vars.end ())
            throw std::runtime_error ("script must assign the final form to 'out'");
        return as_form (it->second, "'out'");
    }
};