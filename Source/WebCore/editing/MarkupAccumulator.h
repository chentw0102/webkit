/*
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#ifndef MarkupAccumulator_h
#define MarkupAccumulator_h

#include "Element.h"
#include "markup.h"
#include <wtf/HashMap.h>
#include <wtf/text/StringBuilder.h>

namespace WebCore {

class Attribute;
class DocumentType;
class Element;
class Node;
class Range;

typedef HashMap<AtomicString, AtomicStringImpl*> Namespaces;

enum EntityMask {
    EntityAmp = 0x0001,
    EntityLt = 0x0002,
    EntityGt = 0x0004,
    EntityQuot = 0x0008,
    EntityNbsp = 0x0010,

    // Non-breaking space needs to be escaped in innerHTML for compatibility reason. See http://trac.webkit.org/changeset/32879
    // However, we cannot do this in a XML document because it does not have the entity reference defined (See the bug 19215).
    EntityMaskInCDATA = 0,
    EntityMaskInPCDATA = EntityAmp | EntityLt | EntityGt,
    EntityMaskInHTMLPCDATA = EntityMaskInPCDATA | EntityNbsp,
    EntityMaskInAttributeValue = EntityAmp | EntityLt | EntityGt | EntityQuot,
    EntityMaskInHTMLAttributeValue = EntityAmp | EntityQuot | EntityNbsp,
};

// FIXME: Noncopyable?
class MarkupAccumulator {
public:
    MarkupAccumulator(Vector<Node*>*, EAbsoluteURLs, const Range* = nullptr, EFragmentSerialization = HTMLFragmentSerialization);
    virtual ~MarkupAccumulator();

    String serializeNodes(Node& targetNode, EChildrenOnly, Vector<QualifiedName>* tagNamesToSkip = nullptr);

    static void appendCharactersReplacingEntities(StringBuilder&, const String&, unsigned, unsigned, EntityMask);

protected:
    static size_t totalLength(const Vector<String>&);
    size_t length() const { return m_markup.length(); }

    void concatenateMarkup(StringBuilder&);

    void appendString(const String&);
    void appendEndTag(const Node& node)
    {
        if (is<Element>(node))
            appendEndTag(downcast<Element>(node));
    }

    virtual void appendEndTag(const Element&);
    virtual void appendCustomAttributes(StringBuilder&, const Element&, Namespaces*);
    virtual void appendText(StringBuilder&, const Text&);
    virtual void appendElement(StringBuilder&, const Element&, Namespaces*);

    void appendStartTag(const Node&, Namespaces* = nullptr);

    void appendOpenTag(StringBuilder&, const Element&, Namespaces*);
    void appendCloseTag(StringBuilder&, const Element&);

    void appendStartMarkup(StringBuilder&, const Node&, Namespaces*);
    void appendEndMarkup(StringBuilder&, const Element&);

    void appendAttributeValue(StringBuilder&, const String&, bool);
    void appendNamespace(StringBuilder&, const AtomicString& prefix, const AtomicString& namespaceURI, Namespaces&, bool allowEmptyDefaultNS = false);
    void appendXMLDeclaration(StringBuilder&, const Document&);
    void appendDocumentType(StringBuilder&, const DocumentType&);
    void appendProcessingInstruction(StringBuilder&, const String& target, const String& data);
    void appendAttribute(StringBuilder&, const Element&, const Attribute&, Namespaces*);
    void appendCDATASection(StringBuilder&, const String&);

    bool shouldAddNamespaceElement(const Element&);
    bool shouldAddNamespaceAttribute(const Attribute&, Namespaces&);
    bool shouldSelfClose(const Element&);
    bool elementCannotHaveEndTag(const Node&);
    EntityMask entityMaskForText(const Text&) const;

    Vector<Node*>* const m_nodes;
    const Range* const m_range;

private:
    String resolveURLIfNeeded(const Element&, const String&) const;
    void appendQuotedURLAttributeValue(StringBuilder&, const Element&, const Attribute&);
    void serializeNodesWithNamespaces(Node& targetNode, EChildrenOnly, const Namespaces*, Vector<QualifiedName>* tagNamesToSkip);
    bool inXMLFragmentSerialization() const { return m_fragmentSerialization == XMLFragmentSerialization; }
    void generateUniquePrefix(QualifiedName&, const Namespaces&);

    StringBuilder m_markup;
    const EAbsoluteURLs m_resolveURLsMethod;
    EFragmentSerialization m_fragmentSerialization;
    unsigned m_prefixLevel;
};

}

#endif
