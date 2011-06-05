/*
 * Exemple d'utilisation de la librairie libxml++ version 1.0.
 */

#ifndef XML_DOCUMENT_H
#define XML_DOCUMENT_H

#include <string>
#include <vector>

#include <WARMUX_base.h>
#include "tool/string_tools.h"

// Forward declaration
typedef struct _xmlNode xmlNode;
typedef struct _xmlDoc xmlDoc;
typedef std::vector<const xmlNode*> xmlNodeArray;
class Color;

class XmlReader
{
  xmlDoc*  doc;
public:
  XmlReader() : doc(NULL) { };
  ~XmlReader();

  // Load an XML document from a file
  bool Load(const std::string &nomfich);

  // Load an XML document from a string in memory
  bool LoadFromString(const std::string &contents);
  std::string ExportToString() const;

  bool IsOk() const { return doc != NULL; }

  // Return the *exploitable* root (use root->parent for the real one) */
  const xmlNode* GetRoot() const;

  // Return the direct children matching name
  static xmlNodeArray GetNamedChildren(const xmlNode* father,
                                       const std::string & name);

  // Return the first child node element named "nodeName" of "father".
  const xmlNode * GetFirstNamedChild(const xmlNode * father,
                                     const std::string & nodeName);

  // Return the current number of children nodes of "father" node.
  uint GetNbChildren(const xmlNode * father);

  // Return the first child node of "father", otherwise NULL
  const xmlNode * GetFirstChild(const xmlNode * father);

  // Return the first next sibling element of "node", otherwise NULL
  const xmlNode * GetNextSibling(const xmlNode * node);

  // Return the name of "node"
  std::string GetNodeName(const xmlNode * node);

  // get a attribute marker
  static bool ReadString(const xmlNode* father,
                         const std::string &name,
                         std::string &output);
  static bool ReadDouble(const xmlNode* father,
                         const std::string &name,
                         Double &output);
  static bool Readfloat(const xmlNode* father,
                        const std::string &name,
                        float &output);
  static bool ReadInt(const xmlNode* father,
                      const std::string &name,
                      int &output);
  static bool ReadUint(const xmlNode* father,
                       const std::string &name,
                       uint &output);
  static bool ReadBool(const xmlNode* father,
                       const std::string &name,
                       bool &output);

  // get an XML element
  static const xmlNode* GetMarker(const xmlNode* x,
                                  const std::string &name);

  // Access to the 'anchor' <[name] name="[attr_name]"> : have to be uniq !
  static const xmlNode* Access(const xmlNode* x,
                               const std::string &name,
                               const std::string &attr_name);

  // Lit un attribut d'un noeud
  static bool ReadStringAttr(const xmlNode* x,
                             const std::string &name,
                             std::string &output);
  static bool ReadDoubleAttr(const xmlNode* x,
                             const std::string &name,
                             Double &output);
  static bool ReadfloatAttr(const xmlNode* x,
                            const std::string &name,
                            float &output);
  static bool ReadIntAttr(const xmlNode* x,
                          const std::string &name,
                          int &output);
  bool ReadPercentageAttr(const xmlNode* node,
                          const std::string & attributName,
                          float & outputValue);
  bool ReadPixelAttr(const xmlNode* node,
                     const std::string & attributName,
                     int & outputValue);
  static bool ReadUintAttr(const xmlNode* x,
                           const std::string &name,
                           uint &output);
  static bool ReadBoolAttr(const xmlNode* x,
                           const std::string &name,
                           bool &output);
  bool ReadHexColorAttr(const xmlNode* node,
                        const std::string & attributName,
                        Color & outputColor);
  bool IsAPercentageAttr(const xmlNode* node,
                         const std::string & attributName);

private:
  // Read marker value
  static bool ReadMarkerValue(const xmlNode* marker, std::string &output);
  void Reset();
};

//-----------------------------------------------------------------------------

class XmlWriter
{
  void Reset();

protected:
  xmlDoc*  m_doc;
  xmlNode* m_root;
  std::string m_filename;
  bool m_save;
  std::string m_encoding;

public:
  XmlWriter() : m_doc(NULL), m_root(NULL), m_save(false) { } ;
  ~XmlWriter();

  bool Create(const std::string &filename, const std::string &root,
              const std::string &version, const std::string &encoding);

  bool IsOk() const { return m_doc && m_root; }

  xmlNode *GetRoot() const;

  xmlNode *WriteElement(xmlNode* x,
                        const std::string &name,
                        const std::string &value);

  void WriteComment(xmlNode* x,
                    const std::string& comment);

  bool Save();

  std::string SaveToString() const;
};

class ConfigElement
{
public:
  typedef enum
  {
    TYPE_INT,
    TYPE_UINT,
    TYPE_BOOL,
    TYPE_DOUBLE
  } Type;

  Type        m_type;
  const char *m_name;
  bool        m_important;

  virtual bool Read(const xmlNode* father) { return true; };
  virtual void Write(XmlWriter& writer, xmlNode* elem) const { };

protected:
  ConfigElement(Type t, const char *n, bool imp = false)
  {
    m_type = t; m_name = n; m_important = imp;
  }
};

class IntConfigElement : public ConfigElement
{
public:
  int       *m_val, m_def, m_min, m_max;
  IntConfigElement(const char *n, int *v, int d)
    : ConfigElement(TYPE_INT, n, false) {  m_val = v; *v = m_def = d; }
  IntConfigElement(const char *n, int *v, int d, int mi, int ma)
    : ConfigElement(TYPE_INT, n, true) { m_val = v; *v = m_def = d; m_min = mi, m_max = ma; ASSERT(d >= mi && d <= ma); }
  bool Read(const xmlNode* father)
  {
    if (!XmlReader::ReadInt(father, m_name, *m_val)) {
      *m_val = m_def;
      return false;
    }
    ASSERT(!m_important || (*m_val >= m_min && *m_val <= m_max));
    return true;
  }
  void Write(XmlWriter& writer, xmlNode* father) const
  {
    writer.WriteElement(father, m_name, int2str(*m_val));
  }
};

class BoolConfigElement : public ConfigElement
{
public:
  bool       *m_val, m_def;
  BoolConfigElement(const char *n, bool *v, bool d, bool imp = false)
    : ConfigElement(TYPE_BOOL, n, imp) { m_val = v; *v = m_def = d; }
  bool Read(const xmlNode* father)
  {
    if (!XmlReader::ReadBool(father, m_name, *m_val)) {
      *m_val = m_def;
      return false;
    }
    return true;
  }
  void Write(XmlWriter& writer, xmlNode* father) const
  {
    writer.WriteElement(father, m_name, bool2str(*m_val));
  }
};

class UintConfigElement : public ConfigElement
{
public:
  uint       *m_val, m_def, m_min, m_max;
  UintConfigElement(const char *n, uint *v, uint d)
    : ConfigElement(TYPE_UINT, n, false) { m_val = v; *v = m_def = d; }
  UintConfigElement(const char *n, uint *v, uint d, uint mi, uint ma)
    : ConfigElement(TYPE_UINT, n, true) { m_val = v; *v = m_def = d; m_min = mi, m_max = ma; ASSERT(d >= mi && d <= ma); }
  bool Read(XmlReader& reader, const xmlNode* father)
  {
    int val;
    if (!reader.ReadInt(father, m_name, val) || val<0) {
      *m_val = m_def;
      return false;
    }
    ASSERT(!m_important || (*m_val >= m_min && *m_val <= m_max));
    *m_val = val;
    return true;
  }
  void Write(XmlWriter& writer, xmlNode* father) const
  {
    writer.WriteElement(father, m_name, uint2str(*m_val));
  }
};

class DoubleConfigElement : public ConfigElement
{
public:
  Double       *m_val, m_def, m_min, m_max;
  DoubleConfigElement(const char *n, Double* v, Double d)
    : ConfigElement(TYPE_DOUBLE, n, false) { m_val = v; *v = m_def = d; }
  DoubleConfigElement(const char *n, Double* v, Double d, Double mi, Double ma)
    : ConfigElement(TYPE_DOUBLE, n, true) { m_val = v; *v = m_def = d; m_min = mi, m_max = ma; ASSERT(d >= mi && d <= ma); }
  bool Read(const xmlNode* father)
  {
    if (!XmlReader::ReadDouble(father, m_name, *m_val)) {
      *m_val = m_def;
      return false;
    }
    ASSERT(!m_important || (*m_val >= m_min && *m_val <= m_max));
    return true;
  }
  void Write(XmlWriter& writer, xmlNode* father) const
  {
    writer.WriteElement(father, m_name, Double2str(*m_val));
  }
};

#endif // XML_DOCUMENT_H