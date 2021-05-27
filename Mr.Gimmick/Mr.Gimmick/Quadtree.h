#pragma once
#include "GameObject.h"

#define MAX_LEVEL               5
#define MAX_OBJECT_IN_REGION    8

class Quadtree
{
private:
    int m_level;
    float l;
    float t;
    float r;
    float b;
    
    Quadtree** m_nodes;
    vector<LPGAMEOBJECT> m_objects_list;
    bool IsContain(CGameObject* object);
    void Split();
public:
    Quadtree(int level, float l, float t, float r, float b)
    {
        this->m_level = level;
        this->l = l;
        this->t = t;
        this->r = r;
        this->b = b;
    }
    ~Quadtree() {};

    void Clear();
    void Insert(CGameObject* object);
    void Retrieve(vector<LPGAMEOBJECT>* return_objects_list, CGameObject* object);
};