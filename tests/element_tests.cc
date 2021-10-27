/*
 * Copyright (C) 2014-2018 Olzhas Rakhimov
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "element.h"

#include <catch2/catch.hpp>

#include "error.h"

namespace scram::mef::test {

namespace {

class NamedElement : public Element {
 public:
  static constexpr const char* kTypeString = "named element";

  using Element::Element;
};

class DummyContainer : public Element,
                       public Container<DummyContainer, NamedElement, false> {
 public:
  static constexpr const char* kTypeString = "dummy container";

  using Element::Element;
};

}  // namespace

TEST_CASE("ElementTest.Name", "[mef::element]") {
  CHECK_THROWS_AS(NamedElement(""), LogicError);

  CHECK_THROWS_AS(NamedElement(".name"), ValidityError);
  CHECK_THROWS_AS(NamedElement("na.me"), ValidityError);
  CHECK_THROWS_AS(NamedElement("name."), ValidityError);

  CHECK_NOTHROW(NamedElement("name"));
  NamedElement el("name");
  CHECK(el.name() == "name");

  // Illegal names by MEF.
  // However, this names don't mess with class and reference invariants.
  CHECK_NOTHROW(NamedElement("na me"));
  CHECK_NOTHROW(NamedElement("na\nme"));
  CHECK_NOTHROW(NamedElement("\tname"));
  CHECK_NOTHROW(NamedElement("name?"));
}

TEST_CASE("ElementTest.Attribute", "[mef::element]") {
  CHECK_THROWS_AS(Attribute("", ""), LogicError);
  CHECK_THROWS_AS(Attribute("name", ""), LogicError);
  CHECK_THROWS_AS(Attribute("", "value"), LogicError);
  CHECK_NOTHROW(Attribute("name", "value", ""));
  CHECK_NOTHROW(Attribute("na me", "value \n in some free form", "weirdo int"));

  Attribute attr("impact", "0.1", "float");
  CHECK_THROWS_AS(attr.name(""), LogicError);
  CHECK_THROWS_AS(attr.value(""), LogicError);
  CHECK_NOTHROW(attr.type(""));
}

TEST_CASE("ElementTest.Label", "[mef::element]") {
  NamedElement el("name");
  CHECK(el.label() == "");
  CHECK_NOTHROW(el.label(""));
  REQUIRE_NOTHROW(el.label("label"));
  CHECK(el.label() == "label");
  CHECK_NOTHROW(el.label("new_label"));
  CHECK_NOTHROW(el.label(""));
}

TEST_CASE("ElementTest.AddAttribute", "[mef::element]") {
  NamedElement el("name");
  Attribute attr("impact", "0.1", "float");
  CHECK(el.GetAttribute(attr.name()) == nullptr);
  REQUIRE_NOTHROW(el.AddAttribute(attr));
  CHECK_THROWS_AS(el.AddAttribute(attr), ValidityError);
  const Attribute* check = el.GetAttribute(attr.name());
  REQUIRE(check);
  CHECK(check->value() == attr.value());
  CHECK(check->name() == attr.name());
  CHECK(check->type() == attr.type());
}

TEST_CASE("ElementTest.SetAttribute", "[mef::element]") {
  NamedElement el("name");
  Attribute attr("impact", "0.1", "float");
  CHECK(el.GetAttribute(attr.name()) == nullptr);
  REQUIRE_NOTHROW(el.SetAttribute(attr));
  CHECK_THROWS_AS(el.AddAttribute(attr), ValidityError);
  const Attribute* check = el.GetAttribute(attr.name());
  REQUIRE(check);
  CHECK(check->value() == attr.value());
  CHECK(check->name() == attr.name());
  CHECK(check->type() == attr.type());

  attr.value("0.2");
  REQUIRE_NOTHROW(el.SetAttribute(attr));
  CHECK(el.attributes().size() == 1);
  check = el.GetAttribute(attr.name());
  REQUIRE(check);
  CHECK(check->value() == attr.value());
  CHECK(check->name() == attr.name());
  CHECK(check->type() == attr.type());
}

TEST_CASE("ElementTest.RemoveAttribute", "[mef::element]") {
  NamedElement el("name");
  Attribute attr("impact", "0.1", "float");

  CHECK_FALSE(el.GetAttribute(attr.name()));
  CHECK(el.attributes().empty());
  CHECK_FALSE(el.RemoveAttribute(attr.name()));

  REQUIRE_NOTHROW(el.AddAttribute(attr));
  CHECK(el.RemoveAttribute(attr.name()));
  CHECK_FALSE(el.GetAttribute(attr.name()));
  CHECK(el.attributes().empty());
}

TEST_CASE("ElementTest.AttributeInheritance", "[mef::element]") {
  NamedElement el("name");
  DummyContainer container("container");
  container.Add(&el);

  CHECK(!el.GetAttribute("impact"));
  CHECK(!container.GetAttribute("impact"));
  container.AddAttribute({"impact", "42"});
  const Attribute* inherited = el.GetAttribute("impact");
  REQUIRE(inherited);
  CHECK(inherited->value() == "42");
  CHECK(el.attributes().empty());

  container.SetAttribute({"impact", "66"});
  inherited = el.GetAttribute("impact");
  REQUIRE(inherited);
  CHECK(inherited->value() == "66");

  CHECK(!el.attributes().count("impact"));
  CHECK(!el.RemoveAttribute("impact"));

  el.AddAttribute({"impact", "13"});
  const Attribute* direct = el.GetAttribute("impact");
  REQUIRE(direct);
  CHECK(direct != inherited);
  CHECK(direct->value() == "13");
  CHECK(el.attributes().count("impact"));

  std::optional<Attribute> removed = el.RemoveAttribute("impact");
  REQUIRE(removed);
  CHECK(removed->value() == "13");
  REQUIRE(container.GetAttribute("impact"));
  CHECK(el.GetAttribute("impact") == container.GetAttribute("impact"));

  container.Remove(&el);
  CHECK(!el.GetAttribute("impact"));
}

namespace {

class TestRole : public Role {
 public:
  using Role::Role;
};

}  // namespace

TEST_CASE("ElementTest.Role", "[mef::element]") {
  CHECK_THROWS_AS(TestRole(RoleSpecifier::kPublic, ".ref"), ValidityError);
  CHECK_THROWS_AS(TestRole(RoleSpecifier::kPublic, "ref."), ValidityError);
  CHECK_NOTHROW(TestRole(RoleSpecifier::kPublic, "ref.name"));

  CHECK_THROWS_AS(TestRole(RoleSpecifier::kPrivate, ""), ValidityError);
  CHECK_NOTHROW(TestRole(RoleSpecifier::kPublic, ""));
}

namespace {

class NameId : public Id {
 public:
  using Id::Id;
};

}  // namespace

TEST_CASE("ElementTest.Id", "[mef::element]") {
  CHECK_THROWS_AS(NameId(""), LogicError);
  CHECK_NOTHROW(NameId("name"));
  CHECK_THROWS_AS(NameId("name", "", RoleSpecifier::kPrivate), ValidityError);

  NameId id_public("name");
  CHECK(id_public.name() == id_public.id());

  NameId id_private("name", "path", RoleSpecifier::kPrivate);
  CHECK(id_private.id() == "path.name");
  CHECK_FALSE(id_private.name() == id_private.id());

  CHECK_FALSE(id_private.id() == id_public.id());

  // Reset.
  id_public.id("id");
  CHECK(id_public.id() == "id");
  CHECK(id_public.name() == "id");
  id_private.id("id");
  CHECK(id_private.id() == "path.id");
  CHECK(id_private.name() == "id");
}

}  // namespace scram::mef::test
