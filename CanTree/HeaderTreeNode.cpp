#include "HeaderTreeNode.h"

#include <QXmlStreamWriter>
#include <QXmlStreamReader>

HeaderTreeNode::HeaderTreeNode(const QVariant &name)
    : CanTreeNode(), m_name(name)
{
}

bool HeaderTreeNode::acceptsChildren() const
{
    return true;
}

QVariant HeaderTreeNode::getData(dataFunction df, int role) const
{
    (void) role;
    if(df == dfName){
        return m_name;
    }else{
        return QVariant();
    }
}

bool HeaderTreeNode::setData(dataFunction df, const QVariant &value)
{
    if(df == dfName){
        m_name = value;
        return true;
    }else{
        return false;
    }
}

void HeaderTreeNode::writeDataToXml(QXmlStreamWriter &writer) const
{
    writer.writeAttribute("name", m_name.toString());
}

void HeaderTreeNode::readDataFromXml(QXmlStreamReader &reader)
{
    m_name = reader.attributes().value("name").toString();
}
