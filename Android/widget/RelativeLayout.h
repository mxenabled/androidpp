//
//  RelativeLayout.h
//  cocos2dx
//
//  Created by Saul Howard on 11/8/13.
//  Copyright (c) 2013 cocos2d-x. All rights reserved.
//

#ifndef __cocos2dx__RelativeLayout__
#define __cocos2dx__RelativeLayout__

#include "AndroidMacros.h"

#include "Android/view/Gravity.h"
#include "Android/view/ViewGroup.h"
#include "Android/widget/RelativeLayoutParams.h"

#include <ui/Rect.h>

#include <functional>
#include <map>
#include <vector>
#include <string.h>
#include <deque>
#include <memory>

using namespace std;

ANDROID_BEGIN

class RelativeLayout : public ViewGroup {

public:
    
    RelativeLayout(Context *context);
    RelativeLayout(Context *context, AttributeSet *attrs);
    
    virtual bool canResolveTextAlignment() { return View::canResolveTextAlignment(); }
    LayoutParams *generateLayoutParams(AttributeSet *attrs);
    virtual int getBaseline();
    int getGravity();
    virtual void requestLayout();
    void setGravity(int gravity);
    void setHorizontalGravity(int horizontalGravity);
    void setIgnoreGravity(string viewId);
    void setVerticalGravity(int verticalGravity);
    virtual bool shouldDelayChildPressedState();
    
protected:
    
    LayoutParams *mLayoutParams = NULL;
    
    virtual bool checkLayoutParams(LayoutParams *p);
    virtual LayoutParams *generateDefaultLayoutParams();
    virtual LayoutParams *generateLayoutParams(LayoutParams *p);
    virtual void onLayout(bool changed, int l, int t, int r, int b);
    virtual void onMeasure(int widthMeasureSpec, int heightMeasureSpec);
    
private:
    
    class DependencyGraph;
    
    /**
     * A node in the dependency graph. A node is a view, its list of dependencies
     * and its list of dependents.
     *
     * A node with no dependent is considered a root of the graph.
     */
    class Node {
        
    public:
        /**
         * The view representing this node in the layout.
         */
        shared_ptr<View> view = NULL;
        
        /**
         * The list of dependents for this node; a dependent is a node
         * that needs this node to be processed first.
         */
        map<Node*, DependencyGraph*> dependents;
        
        /**
         * The list of dependencies for this node.
         */
        map<int64_t, Node*> dependencies;
        
        /*
         * START POOL IMPLEMENTATION
         */
        static vector<Node*> sPool;
        
        static Node *acquire(shared_ptr<View> view) {
            Node *node = sPool.back();
            if (node == NULL) {
                node = new Node();
            } else {
                sPool.pop_back();
            }
            node->view = view;
            return node;
        }
        
        void release() {
            view = NULL;
            dependents.clear();
            dependencies.clear();
            
            sPool.push_back(this);
        }
        /*
         * END POOL IMPLEMENTATION
         */
    };
    
    class DependencyGraph {
        
    public:
        
        /**
         * List of all views in the graph.
         */
        vector<Node*> m_nodes;
        
        /**
         * List of nodes in the graph. Each node is identified by its
         * view id (see View#getId()).
         */
        map<int64_t, Node*> m_keyNodes;
        
        /**
         * Temporary data structure used to build the list of roots
         * for this graph.
         */
        deque<Node*> m_roots;
        
        /**
         * Clears the graph.
         */
        void clear() {
            const int count = m_nodes.size();
            
            for (int i = 0; i < count; i++) {
                m_nodes[i]->release();
            }
            m_nodes.clear();
            
            m_keyNodes.clear();
            m_roots.clear();
        }
        
        /**
         * Adds a view to the graph.
         *
         * @param view The view to be added as a node to the graph.
         */
        void add(shared_ptr<View> view) {
            Node *node = Node::acquire(view);
            
            if (view->getId() != View::NO_ID) {
                size_t id = hash<string>()(view->getId());
                m_keyNodes[id] = node;
            }
            
            m_nodes.push_back(node);
        }
        
        /**
         * Builds a sorted list of views. The sorting order depends on the dependencies
         * between the view. For instance, if view C needs view A to be processed first
         * and view A needs view B to be processed first, the dependency graph
         * is: B -> A -> C. The sorted array will contain views B, A and C in this order.
         *
         * @param sorted The sorted list of views. The length of this array must
         *        be equal to getChildCount().
         * @param rules The list of rules to take into account.
         */
        void getSortedViews(vector<shared_ptr<View>> &sorted, const int rules[], int length) {
            
            deque<Node*> roots = findRoots(rules, length);
            
            int index = 0;
            
            Node *node = NULL;
            while (roots.size() > 0 && (node = roots.back()) != NULL) {
                roots.pop_back();
                
                shared_ptr<View> view = node->view;
                const size_t key = hash<string>()(view->getId());
                
                sorted[index++] = view;
                
                map<Node*, DependencyGraph*> dependents = node->dependents;
                
                typedef map<Node*, DependencyGraph*>::iterator it_type;
                for(it_type iterator = dependents.begin(); iterator != dependents.end(); iterator++) {
                    
                    Node *dependent = iterator->first;
                    map<int64_t, Node*> dependencies = dependent->dependencies;
                    
                    dependencies.erase(key);
                    if (dependencies.size() == 0) {
                        roots.push_back(dependent);
                    }
                }
            }
            
            if (index < sorted.size()) {
                CCLOG("Circular dependencies cannot exist in RelativeLayout");
            }
        }
        
        /**
         * Finds the roots of the graph. A root is a node with no dependency and
         * with [0..n] dependents.
         *
         * @param rulesFilter The list of rules to consider when building the
         *        dependencies
         *
         * @return A list of node, each being a root of the graph
         */
        deque<Node*> &findRoots(const int rulesFilter[], int rulesCount) {
            
            const size_t count = m_nodes.size();
            
            // Find roots can be invoked several times, so make sure to clear
            // all dependents and dependencies before running the algorithm
            for (int i = 0; i < count; i++) {
                Node *node = m_nodes[i];
                node->dependents.clear();
                node->dependencies.clear();
            }
            
            // Builds up the dependents and dependencies for each node of the graph
            for (int i = 0; i < count; i++) {
                Node *node = m_nodes[i];
                
                RelativeLayoutParams *layoutParams = (RelativeLayoutParams*) node->view->getLayoutParams();
                std:vector<int64_t> &rules = layoutParams->m_rules;
                
                // Look only the the rules passed in parameter, this way we build only the
                // dependencies for a specific set of rules
                for (int j = 0; j < rulesCount; j++) {
                    const int64_t rule = rules[rulesFilter[j]];
                    if (rule > 0) {
                        // The node this node depends on
                        Node *dependency = m_keyNodes.find(rule)->second;
                        
                        // Skip unknowns and self dependencies
                        if (dependency == NULL || dependency == node) {
                            continue;
                        }
                        // Add the current node as a dependent
                        dependency->dependents[node] = this;
                        // Add a dependency to the current node
                        node->dependencies[rule] = dependency;
                    }
                }
            }
            
            m_roots.clear();
            
            // Finds all the roots in the graph: all nodes with no dependencies
            for (int i = 0; i < count; i++) {
                Node *node = m_nodes[i];
                if (node->dependencies.size() == 0) m_roots.push_back(node);
            }
            
            return m_roots;
        }
    };
    
    static const int RULES_VERTICAL[5];
    
    static const int RULES_HORIZONTAL[8];

    shared_ptr<View> m_baselineView = NULL;
    bool m_hasBaselineAlignedChild = false;
    
    int m_gravity = Gravity::START | Gravity::TOP;
    Rect m_contentBounds;
    Rect m_selfBounds;
    string m_ignoreGravity;
    
    bool m_dirtyHierarchy;
    vector<shared_ptr<View>> m_sortedHorizontalChildren;
    vector<shared_ptr<View>> m_sortedVerticalChildren;
    DependencyGraph m_graph;
    
    // Compatibility hack. Old versions of the platform had problems
    // with MeasureSpec value overflow and RelativeLayout was one source of them.
    // Some apps came to rely on them. :(
    bool m_allowBrokenMeasureSpecs = false;
    // Compatibility hack. Old versions of the platform would not take
    // margins and padding into account when generating the height measure spec
    // for children during the horizontal measure pass.
    bool m_measureVerticalWithPaddingMargin = false;
    
    // A default width used for RTL measure pass
    /**
     * Value reduced so as not to interfere with View's measurement spec. flags. See:
     * {@link View#MEASURED_SIZE_MASK}.
     * {@link View#MEASURED_STATE_TOO_SMALL}.
     **/
    static const int DEFAULT_WIDTH = 0x00010000;
    
    void alignBaseline(shared_ptr<View> child, RelativeLayoutParams *params);
    void applyHorizontalSizeRules(RelativeLayoutParams *childParams, int myWidth, vector<int64_t> rules);
    void applyVerticalSizeRules(RelativeLayoutParams *childParams, int myHeight);
    static void centerHorizontal(shared_ptr<View> child, RelativeLayoutParams *params, int myWidth);
    static void centerVertical(shared_ptr<View> child, RelativeLayoutParams *params, int myHeight);
    int getChildMeasureSpec(int childStart, int childEnd, int childSize, int startMargin, int endMargin, int startPadding, int endPadding, int mySize);
    shared_ptr<View> getRelatedView(vector<int64_t> rules, int relation);
    int getRelatedViewBaseline(vector<int64_t> rules, int relation);
    RelativeLayoutParams *getRelatedViewParams(vector<int64_t> rules, int relation);
    void initFromAttributes(Context *context, AttributeSet *attrs);
    void measureChild(shared_ptr<View> child, RelativeLayoutParams *params, int myWidth, int myHeight);
    void measureChildHorizontal(shared_ptr<View> child, RelativeLayoutParams *params, int myWidth, int myHeight);
    bool positionChildHorizontal(shared_ptr<View> child, RelativeLayoutParams *params, int myWidth, bool wrapContent);
    bool positionChildVertical(shared_ptr<View> child, RelativeLayoutParams *params, int myHeight, bool wrapContent);
    void sortChildren();
};

ANDROID_END

#endif /* defined(__cocos2dx__RelativeLayout__) */
