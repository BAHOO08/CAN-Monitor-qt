#include "CanTreeModel.h"

#include "HeaderTreeNode.h"
#include "MessageTreeNode.h"
#include "CanTreeNodeFactory.h"


CanTreeModel::CanTreeModel()
    : TreeModel(new HeaderTreeNode())
{
    m_columnFunctions = {
        ColumnRole(dfName,        "Name"),
        ColumnRole(dfID,          "ID (HEX)"),
        ColumnRole(dfDLC,         "DLC"),
        ColumnRole(dfCount,       "Count"),
        ColumnRole(dfPeriod,      "Period (ms)"),
        ColumnRole(dfRawData,     "Raw Data (HEX)"),
        ColumnRole(dfDataDecoded, "Decoded Data"),
        ColumnRole(dfFormat,      "Format String"),
    };
    m_columnCount = m_columnFunctions.count();
}

int CanTreeModel::columnCount(const QModelIndex &parent) const
{
    (void)parent;
    return m_columnCount;
}

QVariant CanTreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole){
        return m_columnFunctions.at(section).name;
    }

    return QVariant();
}

QVariant CanTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || (role != Qt::DisplayRole && role != Qt::EditRole && role != Qt::UserRole))
        return QVariant();

    CanTreeNode *node = static_cast<CanTreeNode*>(nodeForIndex(index));

    return node->getData(m_columnFunctions.at(index.column()).df, role);
}

bool CanTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || role != Qt::EditRole)
        return false;

    isUserModified = true;

    CanTreeNode *node = static_cast<CanTreeNode*>(nodeForIndex(index));
    if(node->setData(m_columnFunctions.at(index.column()).df, value))
    {
        emit(dataChanged(index, index));
        return true;
    }
    else
        return false;
}

Qt::ItemFlags CanTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsDropEnabled;

    Qt::ItemFlags flags = 0;
    switch(m_columnFunctions.at(index.column()).df){
    case dfName:
    case dfFormat:
        flags = Qt::ItemIsEditable ; break;
    default: break;
    }

    return flags | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}

void CanTreeModel::brancheGoingToBeDeleted(TreeNode *node)
{
    unlinkNodes(node);
}

void CanTreeModel::inputMessage(can_message_t cmsg){
    uint32_t uid = CanUniqueID(&cmsg).val;

    if(map.contains(uid)){
        MessageTreeNode * node = map[uid];
        node->update(&cmsg);
        emitDataChanged(node, 2, columnCount()-2);
    }
    else
    {
        MessageTreeNode * node = new MessageTreeNode(&cmsg);
        map[uid] = node;
        insertBranche(rootNode(), -1, node);
    }
}

bool CanTreeModel::linkNodesAndRemoveDuplicates(TreeNode * node)
{
    MessageTreeNode *mtn = dynamic_cast<MessageTreeNode *>(node);

    if(mtn){
        uint32_t uid = CanUniqueID(mtn->getId(), mtn->getIDE(), mtn->getRTR()).val;

        if(map.contains(uid))
            return false; // return false if this is a duplicate message node

        map[uid] = mtn; // otherwise link it and return true
        return true;
    } else {
        // HeaderTreeNode
        for(int i=0; i<node->childCount(); ) {
            if(!linkNodesAndRemoveDuplicates(node->child(i)))
                node->removeChild(i); // remove duplicate message node from this one
            else
                i++;
        }
        return true;
    }
}

void CanTreeModel::unlinkNodes(TreeNode * node)
{
    auto map = &this->map;
    node->for_tree( [map](TreeNode * tn) -> void{
        if(auto mtn = dynamic_cast<MessageTreeNode*>(tn))
        {
            uint32_t uid = CanUniqueID(mtn->getId(), mtn->getIDE(), mtn->getRTR()).val;
            if(map->contains(uid))
                map->remove(uid);
        }
    });
}

void CanTreeModel::writeTreeToXml(QXmlStreamWriter &writer)
{
    CanTreeNodeFactory factory;
    writer.writeStartDocument();
    writer.writeStartElement("CanTree");
    static_cast<XmlTreeNode*>(rootNode())->writeBranchToXml(writer, factory);
    writer.writeEndElement();
    writer.writeEndDocument();
}

bool CanTreeModel::readTreeFromXml(QXmlStreamReader &reader)
{

    if (!reader.readNextStartElement())
        return false;
    if (reader.name() != "CanTree")
        return false;

    CanTreeNodeFactory factory;
    XmlTreeNode * loadRoot = XmlTreeNode::readBranchFromXml(reader, factory);

    linkNodesAndRemoveDuplicates(loadRoot);
    for(int i=0; i < loadRoot->childCount(); i++)
    {
        insertBranche(rootNode(), -1, loadRoot->child(i));
    }

    return true;
}



CanUniqueID::CanUniqueID(const can_message_t * cmsg)
{
    val = cmsg->id & 0x1fffffff;
    val |= cmsg->IDE << 31;
    val |= cmsg->RTR << 30;
}

CanUniqueID::CanUniqueID(uint32_t id, bool IDE, bool RTR)
{
    val = id & 0x1fffffff;
    val |= (IDE?1:0) << 31;
    val |= (RTR?1:0) << 30;
}
