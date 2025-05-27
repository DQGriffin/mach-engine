//
// Created by Dquavius Griffin on 5/27/25.
//

#ifndef BULLETDEBUGDRAWER_H
#define BULLETDEBUGDRAWER_H

#include <glm/glm.hpp>
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <format>
#include <iostream>

#include "DebugLineRenderer.h"
#include "../../logging/logging.h"

namespace mach::renderer
{
    class BulletDebugDrawer : public btIDebugDraw {
    private:
        DebugLineRenderer* m_lineRenderer;
    public:
        int m_debugMode = DBG_DrawWireframe;

        BulletDebugDrawer(DebugLineRenderer* lineRenderer)
        {
            m_lineRenderer = lineRenderer;
        }

        void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override {
            // Convert btVector3 to your engine's types (e.g. glm::vec3) and draw a line
            m_lineRenderer->addLine(glm::vec3(from.x(), from.y(), from.z()),
                          glm::vec3(to.x(), to.y(), to.z()),
                          glm::vec3(color.x(), color.y(), color.z()));
        }

        void drawContactPoint(const btVector3& point, const btVector3& normal,
                              btScalar distance, int lifeTime, const btVector3& color) override {
            // Optional: visualize contact normals
        }

        void reportErrorWarning(const char* warningString) override {
            LOG_ERROR(std::format("[Bullet Warning] {}", warningString));
        }

        void draw3dText(const btVector3& location, const char* textString) override {
            // Optional
        }

        void setDebugMode(int debugMode) override {
            m_debugMode = debugMode;
        }

        int getDebugMode() const override {
            return m_debugMode;
        }
    };
}
#endif //BULLETDEBUGDRAWER_H
