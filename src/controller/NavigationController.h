/**
 * ============================================================================
 * File: src/controller/NavigationController.h
 * Description: Navigation state management for QML
 * ============================================================================
 */

#ifndef NAVIGATIONCONTROLLER_H
#define NAVIGATIONCONTROLLER_H

#include <QObject>
#include <QString>

/**
 * @class NavigationController
 * @brief Manages navigation state and history
 * 
 * Handles page transitions and back navigation
 */
class NavigationController : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(QString currentPage READ currentPage NOTIFY currentPageChanged)
    Q_PROPERTY(bool canGoBack READ canGoBack NOTIFY canGoBackChanged)

public:
    explicit NavigationController(QObject* parent = nullptr);
    ~NavigationController() override = default;

    int currentIndex() const { return m_currentIndex; }
    QString currentPage() const { return m_currentPage; }
    bool canGoBack() const { return m_previousIndex >= 0; }

    void setCurrentIndex(int index);

    Q_INVOKABLE void navigateTo(int index);
    Q_INVOKABLE void goBack();
    Q_INVOKABLE void goToDashboard();
    Q_INVOKABLE void goToSettings();

signals:
    void currentIndexChanged();
    void currentPageChanged();
    void canGoBackChanged();
    void navigationRequested(int index);

private:
    QString indexToPageName(int index) const;

private:
    int m_currentIndex;
    int m_previousIndex;
    QString m_currentPage;
};

#endif // NAVIGATIONCONTROLLER_H