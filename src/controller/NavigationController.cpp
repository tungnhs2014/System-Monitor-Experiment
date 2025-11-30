/**
 * ============================================================================
 * File: src/controller/NavigationController.cpp
 * Description: Navigation controller implementation
 * ============================================================================
 */

#include "NavigationController.h"
#include "Constants.h"

NavigationController::NavigationController(QObject* parent)
    : QObject(parent)
    , m_currentIndex(App::NavIndex::DASHBOARD)
    , m_previousIndex(-1)
    , m_currentPage("Dashboard")
{
}

void NavigationController::setCurrentIndex(int index)
{
    if (m_currentIndex != index && index >= 0 && index <= App::NavIndex::SETTINGS) {
        m_previousIndex = m_currentIndex;
        m_currentIndex = index;
        m_currentPage = indexToPageName(index);
        
        emit currentIndexChanged();
        emit currentPageChanged();
        emit canGoBackChanged();
    }
}

void NavigationController::navigateTo(int index)
{
    setCurrentIndex(index);
    emit navigationRequested(index);
}

void NavigationController::goBack()
{
    if (m_previousIndex >= 0) {
        int temp = m_currentIndex;
        m_currentIndex = m_previousIndex;
        m_previousIndex = -1;  // Clear history after going back
        m_currentPage = indexToPageName(m_currentIndex);
        
        emit currentIndexChanged();
        emit currentPageChanged();
        emit canGoBackChanged();
        emit navigationRequested(m_currentIndex);
    }
    else {
        // Default: go to DashBoard
        navigateTo(App::NavIndex::DASHBOARD);
    }
}

void NavigationController::goToDashboard()
{
    navigateTo(App::NavIndex::DASHBOARD);
}

void NavigationController::goToSettings()
{
    navigateTo(App::NavIndex::SETTINGS);
}

QString NavigationController::indexToPageName(int index) const
{
    switch (index) {
        case App::NavIndex::DASHBOARD: return "Dashboard";
        case App::NavIndex::CPU:       return "CPU Detail";
        case App::NavIndex::MEMORY:    return "Memory Detail";
        case App::NavIndex::STORAGE:   return "Storage Detail";
        case App::NavIndex::NETWORK:   return "Network Detail";
        case App::NavIndex::SETTINGS:  return "Settings";
        default:                       return "Unknown";
    }
}
