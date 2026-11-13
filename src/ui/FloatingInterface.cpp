#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include "../layout/AutoLayoutGenModule.hpp"
#include <algorithm>
#include <string>

using namespace geode::prelude;

namespace {
enum class Page { Home, Decoration, Layout, TriggerMarket, ObjectMarket, Tools };

class DLPanel : public CCLayerColor {
    Page m_page = Page::Home;
    LayoutProfile m_profile;
    std::string m_status = "Escolha uma ferramenta.";
    std::string m_preset = "NULL";

    void addLabel(CCNode* parent, std::string const& text, CCPoint pos, float scale = .55f) {
        auto label = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
        if (!label) return;
        label->setScale(scale);
        label->setPosition(pos);
        parent->addChild(label);
    }

    void rebuild() {
        this->removeAllChildrenWithCleanup(true);
        auto size = CCDirector::sharedDirector()->getWinSize();
        auto w = std::min(410.f, size.width - 28.f);
        auto h = std::min(270.f, size.height - 28.f);
        this->setContentSize(size);
        auto panel = CCLayerColor::create(ccc4(22, 20, 38, 248), w, h);
        panel->setPosition((size.width - w) / 2.f, (size.height - h) / 2.f);
        this->addChild(panel, 1);

        auto title = CCLabelBMFont::create("DL MEGA UTILS", "bigFont.fnt");
        title->setScale(.72f);
        title->setPosition(w / 2.f, h - 22.f);
        panel->addChild(title);

        auto closeLabel = CCLabelBMFont::create("X", "bigFont.fnt");
        closeLabel->setScale(.72f);
        auto close = CCMenuItemLabel::create(closeLabel, this, menu_selector(DLPanel::onAction));
        close->setTag(0);
        auto topMenu = CCMenu::create(close, nullptr);
        topMenu->setPosition(CCPointZero);
        close->setPosition(w - 19.f, h - 20.f);
        panel->addChild(topMenu, 5);

        auto makeButton = [&](std::string const& text, int tag, float y) {
            auto label = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
            if (!label) return;
            label->setScale(.48f);
            auto item = CCMenuItemLabel::create(label, this, menu_selector(DLPanel::onAction));
            item->setTag(tag);
            auto menu = CCMenu::create(item, nullptr);
            menu->setPosition(CCPointZero);
            panel->addChild(menu, 4);
            item->setPosition(w / 2.f, y);
        };

        if (m_page == Page::Home) {
            makeButton("AUTO DECORATION", 1, h - 66.f);
            makeButton("AUTO LAYOUT GEN", 2, h - 101.f);
            makeButton("TRIGGER MARKET", 3, h - 136.f);
            makeButton("OBJECT MARKET", 4, h - 171.f);
            makeButton("EDITOR TOOLS", 5, h - 206.f);
        } else {
            makeButton("< VOLTAR", 10, 24.f);
            if (m_page == Page::Decoration) {
                addLabel(panel, "PRESET: " + m_preset, { w / 2.f, h - 61.f });
                makeButton("NULL", 16, h - 100.f);
                makeButton("HELL", 17, h - 132.f);
                makeButton("TECHNO", 18, h - 164.f);
                makeButton("MODERN", 19, h - 196.f);
                addLabel(panel, "Preset selecionado; aplicacao no editor pendente.", { w / 2.f, 48.f }, .34f);
            } else if (m_page == Page::Layout) {
                auto diff = static_cast<int>(m_profile.difficulty);
                auto len = static_cast<int>(m_profile.length);
                auto sync = static_cast<int>(m_profile.sync);
                const char* diffs[] = { "EASY", "NORMAL", "HARD", "INSANE", "DEMON" };
                const char* lengths[] = { "SHORT", "MEDIUM", "LONG" };
                const char* syncs[] = { "LOW", "NORMAL", "HIGH" };
                makeButton(std::string("DIFFICULTY: ") + diffs[diff], 11, h - 65.f);
                makeButton(std::string("LENGTH: ") + lengths[len], 12, h - 98.f);
                makeButton(std::string("SYNC: ") + syncs[sync], 13, h - 131.f);
                makeButton(std::string("DROPS: ") + (m_profile.includeDrops ? "ON" : "OFF"), 14, h - 164.f);
                makeButton("GENERATE PLAN", 15, h - 197.f);
                addLabel(panel, m_status, { w / 2.f, 47.f }, .32f);
            } else if (m_page == Page::TriggerMarket) {
                addLabel(panel, "TRIGGER MARKET", { w / 2.f, h - 70.f });
                addLabel(panel, "Compartilhamento online ainda nao conectado.", { w / 2.f, h - 115.f }, .36f);
                addLabel(panel, "Nenhum download ou envio sera simulado.", { w / 2.f, h - 145.f }, .34f);
            } else if (m_page == Page::ObjectMarket) {
                addLabel(panel, "OBJECT MARKET", { w / 2.f, h - 70.f });
                addLabel(panel, "Biblioteca online ainda nao conectada.", { w / 2.f, h - 115.f }, .36f);
                addLabel(panel, "Nenhum objeto sera baixado ficticiamente.", { w / 2.f, h - 145.f }, .34f);
            } else if (m_page == Page::Tools) {
                addLabel(panel, "EDITOR TOOLS", { w / 2.f, h - 70.f });
                addLabel(panel, "Ferramentas serao adicionadas aqui.", { w / 2.f, h - 115.f }, .38f);
                addLabel(panel, "Esta tela e apenas o ponto de entrada.", { w / 2.f, h - 145.f }, .34f);
            }
        }
    }

    void open(Page page) {
        m_page = page;
        rebuild();
    }

    void onAction(CCObject* sender) {
        auto tag = sender->getTag();
        if (tag == 0) { this->removeFromParentAndCleanup(true); return; }
        if (tag == 10) { open(Page::Home); return; }
        if (tag == 1) { open(Page::Decoration); return; }
        if (tag == 2) { open(Page::Layout); return; }
        if (tag == 3) { open(Page::TriggerMarket); return; }
        if (tag == 4) { open(Page::ObjectMarket); return; }
        if (tag == 5) { open(Page::Tools); return; }
        if (tag >= 16 && tag <= 19) {
            const char* presets[] = { "NULL", "HELL", "TECHNO", "MODERN" };
            m_preset = presets[tag - 16];
            rebuild();
            return;
        }
        if (tag == 11) {
            m_profile.difficulty = static_cast<LayoutDifficulty>((static_cast<int>(m_profile.difficulty) + 1) % 5);
        } else if (tag == 12) {
            m_profile.length = static_cast<LayoutLength>((static_cast<int>(m_profile.length) + 1) % 3);
        } else if (tag == 13) {
            m_profile.sync = static_cast<SyncIntensity>((static_cast<int>(m_profile.sync) + 1) % 3);
        } else if (tag == 14) {
            m_profile.includeDrops = !m_profile.includeDrops;
        } else if (tag == 15) {
            auto plan = AutoLayoutGenModule::generatePlan(m_profile);
            auto drops = std::count_if(plan.begin(), plan.end(), [](LayoutSegment const& s) { return s.isDrop; });
            m_status = "Plano criado: " + std::to_string(plan.size()) + " segmentos / " +
                std::to_string(drops) + " drops. Ainda nao insere objetos.";
        }
        rebuild();
    }

public:
    static DLPanel* create() {
        auto ret = new DLPanel();
        auto size = CCDirector::sharedDirector()->getWinSize();
        if (ret && ret->initWithColor(ccc4(0, 0, 0, 150), size.width, size.height)) {
            ret->autorelease();
            ret->rebuild();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

class DLFloatingOrb : public CCLayer {
    CCSprite* m_orb = nullptr;
    bool m_dragging = false;
    bool m_moved = false;
    CCPoint m_touchStart;
    CCPoint m_orbStart;
    void togglePanel() {
        auto existing = this->getChildByID("dl-panel");
        if (existing) {
            existing->removeFromParentAndCleanup(true);
            return;
        }
        auto panel = DLPanel::create();
        if (panel) {
            panel->setID("dl-panel");
            this->addChild(panel, 20);
        }
    }

public:
    static DLFloatingOrb* create() {
        auto ret = new DLFloatingOrb();
        if (ret && ret->init()) { ret->autorelease(); return ret; }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    bool init() override {
        if (!CCLayer::init()) return false;
        m_orb = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
        if (!m_orb) m_orb = CCSprite::createWithSpriteFrameName("GJ_likeBtn_001.png");
        if (!m_orb) return true;
        m_orb->setScale(1.25f);
        auto win = CCDirector::sharedDirector()->getWinSize();
        m_orb->setPosition(38.f, win.height * .55f);
        this->addChild(m_orb, 2);
        auto orbText = CCLabelBMFont::create("DL", "bigFont.fnt");
        if (orbText) { orbText->setScale(.30f); orbText->setPosition(m_orb->getContentSize() / 2.f); m_orb->addChild(orbText, 5); }
        return true;
    }

    void onEnter() override {
        CCLayer::onEnter();
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -500, true);
    }

    void onExit() override {
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
        CCLayer::onExit();
    }

    bool ccTouchBegan(CCTouch* touch, CCEvent*) override {
        if (this->getChildByID("dl-panel") || !m_orb) return false;
        auto p = this->convertToNodeSpace(touch->getLocation());
        if (!m_orb->boundingBox().containsPoint(p)) return false;
        m_dragging = true;
        m_moved = false;
        m_touchStart = p;
        m_orbStart = m_orb->getPosition();
        return true;
    }

    void ccTouchMoved(CCTouch* touch, CCEvent*) override {
        if (!m_dragging || !m_orb) return;
        auto p = this->convertToNodeSpace(touch->getLocation());
        auto delta = p - m_touchStart;
        if (std::abs(delta.x) + std::abs(delta.y) > 7.f) m_moved = true;
        auto win = CCDirector::sharedDirector()->getWinSize();
        auto x = std::clamp(m_orbStart.x + delta.x, 24.f, win.width - 24.f);
        auto y = std::clamp(m_orbStart.y + delta.y, 24.f, win.height - 24.f);
        m_orb->setPosition(x, y);
    }

    void ccTouchEnded(CCTouch*, CCEvent*) override {
        if (!m_dragging) return;
        m_dragging = false;
        if (!m_moved) togglePanel();
    }
};

void addFloatingOrb(CCNode* parent) {
    if (!parent || parent->getChildByID("dl-floating-orb")) return;
    auto orb = DLFloatingOrb::create();
    if (!orb) return;
    orb->setID("dl-floating-orb");
    parent->addChild(orb, 999);
}
}

class $modify(DLMegaMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;
        addFloatingOrb(this);
        return true;
    }
};

class $modify(DLMegaEditorUI, EditorUI) {
    bool init(LevelEditorLayer* editor) {
        if (!EditorUI::init(editor)) return false;
        addFloatingOrb(this);
        return true;
    }
};
