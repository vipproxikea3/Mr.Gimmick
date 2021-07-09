#include "Quadtree.h"
#include "Utils.h"

void Quadtree::Clear()
{
    // Clear all nodes
    if (m_nodes)
    {
        for (int i = 0; i < 4; i++)
        {
            m_nodes[i]->Clear();
            delete m_nodes[i];
            m_nodes[i] = nullptr;
        }
        delete[] m_nodes;
        m_nodes = nullptr;
    }

    // Clear current Quadtree
    m_objects_list.clear();
}

bool Quadtree::IsContain(CGameObject* entity)
{
    float ol,ot,or,ob;
    entity->GetBoundingBox(ol,ot,or,ob);

    return ! (or < l || ot < b || ol > r || ob > t );
}

void Quadtree::Split()
{
    m_nodes = new Quadtree*[4];

    m_nodes[0] = new Quadtree(m_level + 1, l, t, l + (r - l) / 2, t - (t - b) / 2);
    m_nodes[1] = new Quadtree(m_level + 1, l + (r - l) / 2, t, r, t - (t - b) / 2);
    m_nodes[2] = new Quadtree(m_level + 1, l, t - (t - b) / 2, l + (r - l) / 2, b);
    m_nodes[3] = new Quadtree(m_level + 1, l + (r - l) / 2, t - (t - b) / 2, r, b);
}

void Quadtree::Insert(CGameObject* entity)
{
    // Insert entity into corresponding nodes
    if (m_nodes)
    {
        if (m_nodes[0]->IsContain(entity))
            m_nodes[0]->Insert(entity);
        if (m_nodes[1]->IsContain(entity))
            m_nodes[1]->Insert(entity);
        if (m_nodes[2]->IsContain(entity))
            m_nodes[2]->Insert(entity);
        if (m_nodes[3]->IsContain(entity))
            m_nodes[3]->Insert(entity);

        return; // Return here to ignore rest.
    }

    // Insert entity into current quadtree
    if (this->IsContain(entity))
        m_objects_list.push_back(entity);

    // Split and move all objects in list into it’s corresponding nodes
    if (m_objects_list.size() > MAX_OBJECT_IN_REGION && m_level < MAX_LEVEL)
    {
        Split();

        while (!m_objects_list.empty())
        {
            if (m_nodes[0]->IsContain(m_objects_list.back()))
                m_nodes[0]->Insert(m_objects_list.back());
            if (m_nodes[1]->IsContain(m_objects_list.back()))
                m_nodes[1]->Insert(m_objects_list.back());
            if (m_nodes[2]->IsContain(m_objects_list.back()))
                m_nodes[2]->Insert(m_objects_list.back());
            if (m_nodes[3]->IsContain(m_objects_list.back()))
                m_nodes[3]->Insert(m_objects_list.back());

            m_objects_list.pop_back();
        }
    }
}

void Quadtree::Retrieve(vector<LPGAMEOBJECT>* return_objects_list, CGameObject* entity)
{
    if (m_nodes)
    {
        if (m_nodes[0]->IsContain(entity))
            m_nodes[0]->Retrieve(return_objects_list, entity);
        if (m_nodes[1]->IsContain(entity))
            m_nodes[1]->Retrieve(return_objects_list, entity);
        if (m_nodes[2]->IsContain(entity))
            m_nodes[2]->Retrieve(return_objects_list, entity);
        if (m_nodes[3]->IsContain(entity))
            m_nodes[3]->Retrieve(return_objects_list, entity);

        return; // Return here to ignore rest.
    }

    // Add all entities in current region into return_objects_list
    if (this->IsContain(entity))
    {
        for (size_t i = 0; i < m_objects_list.size(); i++)
        {
            if (entity == m_objects_list[i])
                continue;
            return_objects_list->push_back(m_objects_list[i]);
        }
    }
}