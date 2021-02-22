#include "GeometricMesh.h"
#include "CollidableNode.h"
#include "glm/gtx/intersect.hpp"

CollidableNode::CollidableNode(void) :
    GeometryNode()
{ /* Empty */ }

CollidableNode::~CollidableNode(void) { /* Empty */ }

void CollidableNode::Init(class GeometricMesh* mesh)
{
    this->triangles.resize(mesh->vertices.size() / 3);

    for (size_t t = 0; t < this->triangles.size(); ++t)
    {
        this->triangles[t].v0 = mesh->vertices[t * 3 + 0];
        this->triangles[t].v1 = mesh->vertices[t * 3 + 1];
        this->triangles[t].v2 = mesh->vertices[t * 3 + 2];
    }

    super::Init(mesh);
}

bool CollidableNode::intersectRay(
    const glm::vec3& pOrigin_wcs,
    const glm::vec3& pDir_wcs,
    const glm::mat4& pWorldMatrix,
    float & pIsectDist,
    float pTmax,
    float pTmin)
{
    if(pTmax < pTmin || glm::length(pDir_wcs) < glm::epsilon<float>()) return false;

    const glm::vec3 normDir = glm::normalize(pDir_wcs);

    const glm::mat4 wordToModel = glm::inverse(pWorldMatrix * super::app_model_matrix);
    const glm::vec3 o_local = wordToModel * glm::vec4(pOrigin_wcs, 1.f);
    const glm::vec3 d_local = glm::normalize(glm::vec3(wordToModel * glm::vec4(normDir, 0.f)));

    pIsectDist = pTmax;
    float_t curMin = pTmax;
    bool found_isect = false;
    glm::vec3 isect(1.f);

    for (auto& tr : this->triangles)
    {
        glm::vec3 barycoord;

        if (glm::intersectRayTriangle(o_local, d_local, tr.v0, tr.v1, tr.v2, barycoord))
        {
            const glm::vec3 tmp_isect = tr.v0 * barycoord.x + tr.v1 * barycoord.y + tr.v2 * barycoord.z;
            float dist = glm::distance(o_local, tmp_isect);

            if (dist < curMin && dist >= pTmin)
            {
                curMin = dist;
                found_isect = true;
                isect = tmp_isect;
            }
        }
    }

    if (found_isect)
    {
        glm::vec3 isec_wcs = pWorldMatrix * super::app_model_matrix * glm::vec4(isect, 1.f);
        pIsectDist = glm::distance(pOrigin_wcs, isec_wcs);
    }

    return found_isect;
}