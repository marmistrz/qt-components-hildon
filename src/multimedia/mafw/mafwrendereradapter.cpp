/**************************************************************************
    This file is part of Open MediaPlayer
    Copyright (C) 2010-2011 Nicolai Hess

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**************************************************************************/

#include "mafwrendereradapter.h"

MafwRendererAdapter::MafwRendererAdapter()
{
    this->mafw_renderer = NULL;
    this->playback = NULL;
    memset(&GVolume, 0, sizeof(GVolume));
    g_value_init (&GVolume, G_TYPE_UINT);
    g_warning("start\n");
    mafw_registry = MAFW_REGISTRY(mafw_registry_get_instance());
    mafw_shared_init(mafw_registry, NULL);
    findRenderer();
    connectRegistrySignals();
}

void MafwRendererAdapter::findRenderer()
{
    if(mafw_registry)
    {
        GList* renderer_list = mafw_registry_get_renderers(mafw_registry);
        if(renderer_list)
        {
            GList* renderer_elem = renderer_list;
            while(renderer_elem)
            {
                MafwRenderer* mafw_renderer = MAFW_RENDERER(renderer_elem->data);
                g_warning("renderer: %s\n", mafw_extension_get_name(MAFW_EXTENSION(mafw_renderer)));

                if(g_strcmp0(mafw_extension_get_name(MAFW_EXTENSION(mafw_renderer)),MEDIAPLAYER_RENDERER) == 0)
                {
                    g_object_ref(mafw_renderer);
                    this->mafw_renderer = mafw_renderer;
                    connectRendererSignals();
                }
            }
        }
        else
        {
            g_warning("no renderer\n");
        }
    }
    else
    {
        g_warning("no rgistry\n");
    }
}

void MafwRendererAdapter::enablePlayback(bool enable, bool compatible)
{
    if (enable) {
        if (playback) {
            if (compatiblePlayback != compatible) {
                enablePlayback(false);
                enablePlayback(true, compatible);
            }
        } else {
            compatiblePlayback = compatible;
            connect(this, SIGNAL(signalGetStatus(MafwPlaylist*,uint,MafwPlayState,const char*,QString)),
                    this, SLOT(initializePlayback(MafwPlaylist*,uint,MafwPlayState,const char*,QString)));
            getStatus();
        }
    } else {
        if (playback) {
            pb_playback_destroy(playback);
            playback = NULL;
        }
    }
}

void MafwRendererAdapter::initializePlayback(MafwPlaylist*, uint, MafwPlayState state, const char*, QString)
{
    disconnect(this, SIGNAL(signalGetStatus(MafwPlaylist*,uint,MafwPlayState,const char*,QString)),
               this, SLOT(initializePlayback(MafwPlaylist*,uint,MafwPlayState,const char*,QString)));

    playback = pb_playback_new(dbus_g_connection_get_connection(dbus_g_bus_get(DBUS_BUS_SESSION, NULL)), PB_CLASS_MEDIA,
                               state == Playing && !compatiblePlayback ? PB_STATE_PLAY : PB_STATE_STOP,
                               playback_state_req_handler, NULL);
}

void MafwRendererAdapter::playback_state_req_callback(pb_playback_t *pb, pb_state_e granted_state, const char *reason, pb_req_t *req, void *data)
{
// Waiting for an opinion from libplayback can take so long that there can be
// a significant delay between disconnecting a headset and pausing music and
// play/pause button also seems a bit laggy. Because of that it might be a good
// idea to not wait for permission, but rather do what we have to do and later
// only tell the playback manager what we wanted.
#ifdef LIBPLAYBACK_FULL
    MafwRendererAdapter *adapter = static_cast<req_state_cb_payload*>(data)->adapter;

    if (adapter->mafw_renderer) {
        if (granted_state == PB_STATE_STOP) {
            switch (static_cast<req_state_cb_payload*>(data)->action) {
                case Pause: mafw_renderer_pause(adapter->mafw_renderer, MafwRendererSignalHelper::pause_playback_cb, adapter); break;
                case Stop: mafw_renderer_stop(adapter->mafw_renderer, MafwRendererSignalHelper::stop_playback_cb, adapter); break;
                default: break;
            }
        }
        else if (granted_state == PB_STATE_PLAY) {
            switch (static_cast<req_state_cb_payload*>(data)->action) {
                case Play: mafw_renderer_play(adapter->mafw_renderer, MafwRendererSignalHelper::play_playback_cb, adapter); break;
                case Resume: mafw_renderer_resume(adapter->mafw_renderer, MafwRendererSignalHelper::resume_playback_cb, adapter); break;
                default: break;
            }
        }
        else { // granted_state == PB_STATE_NONE
            qDebug() << "State request denied:" << reason;
        }
    }

    pb_playback_req_completed(pb, req);
    delete static_cast<req_state_cb_payload*>(data);
#else
    Q_UNUSED(granted_state);
    Q_UNUSED(reason);
    Q_UNUSED(data);

    pb_playback_req_completed(pb, req);
#endif
}

void MafwRendererAdapter::playback_state_req_handler(pb_playback_t *pb, pb_state_e req_state, pb_req_t *ext_req, void *data)
{
    // This could be used to handle incoming calls.
    // Currently that is accomplished using MCE in MainWindow.
    Q_UNUSED(pb);
    Q_UNUSED(req_state);
    Q_UNUSED(ext_req);
    Q_UNUSED(data);
}

void MafwRendererAdapter::connectRegistrySignals()
{
    g_signal_connect(mafw_registry,
                     "renderer_added",
                     G_CALLBACK(&onRendererAdded),
                     static_cast<void*>(this));

    g_signal_connect(mafw_registry,
                     "renderer_removed",
                     G_CALLBACK(&onRendererRemoved),
                     static_cast<void*>(this));
}

void MafwRendererAdapter::connectRendererSignals()
{
#ifdef DEBUG_MAFW
    qDebug() << "connect renderer signals";
#endif
    g_signal_connect(mafw_renderer,
                     "buffering-info",
                     G_CALLBACK(&onBufferingInfo),
                     static_cast<void*>(this));
    g_signal_connect(mafw_renderer,
                     "media-changed",
                     G_CALLBACK(&onMediaChanged),
                     static_cast<void*>(this));
    g_signal_connect(mafw_renderer,
                     "metadata-changed",
                     G_CALLBACK(&onMetadataChanged),
                     static_cast<void*>(this));
    g_signal_connect(mafw_renderer,
                     "playlist-changed",
                     G_CALLBACK(&onPlaylistChanged),
                     static_cast<void*>(this));
    g_signal_connect(mafw_renderer,
                     "state-changed",
                     G_CALLBACK(&onStateChanged),
                     static_cast<void*>(this));
}

void MafwRendererAdapter::disconnectRendererSignals()
{

}

void MafwRendererAdapter::onRendererAdded(MafwRegistry*,
                                          GObject* renderer,
                                          gpointer user_data)
{
    if(g_strcmp0(mafw_extension_get_name(MAFW_EXTENSION(renderer)), MEDIAPLAYER_RENDERER) == 0)
    {
        g_object_ref(renderer);
        static_cast<MafwRendererAdapter*>(user_data)->mafw_renderer = MAFW_RENDERER(renderer);
        static_cast<MafwRendererAdapter*>(user_data)->connectRendererSignals();
        emit static_cast<MafwRendererAdapter*>(user_data)->rendererReady();
    }
}


void MafwRendererAdapter::onRendererRemoved(MafwRegistry*,
                                            GObject* renderer,
                                            gpointer user_data)
{
    if(g_strcmp0(mafw_extension_get_name(MAFW_EXTENSION(renderer)), MEDIAPLAYER_RENDERER) == 0)
    {
        g_object_unref(renderer);
        static_cast<MafwRendererAdapter*>(user_data)->mafw_renderer = MAFW_RENDERER(renderer);
        static_cast<MafwRendererAdapter*>(user_data)->disconnectRendererSignals();
    }
}

void MafwRendererAdapter::onBufferingInfo(MafwRenderer*,
                                          gfloat status,
                                          gpointer user_data)
{
#ifdef DEBUG_MAFW
    qDebug() << "On buffering info";
#endif
    emit static_cast<MafwRendererAdapter*>(user_data)->bufferingInfo(status);
}

void MafwRendererAdapter::onMediaChanged(MafwRenderer*,
                                         gint index,
                                         gchar* object_id,
                                         gpointer user_data)
{
#ifdef DEBUG_MAFW
    qDebug() << "On media changed";
#endif
    emit static_cast<MafwRendererAdapter*>(user_data)->mediaChanged(index, object_id);
}

void MafwRendererAdapter::onMetadataChanged(MafwRenderer*,
                                            gchar* name,
                                            GValueArray* value,
                                            gpointer user_data)
{
#ifdef DEBUG_MAFW
    qDebug() << "On Metadata Changed" << name;
#endif
    if(value->n_values == 1)
    {
        GValue* v = g_value_array_get_nth(value, 0);
        switch(G_VALUE_TYPE(v))
        {
            case G_TYPE_STRING:
            {
                const gchar* str_value = g_value_get_string(v);
                QVariant data = QVariant(QString::fromUtf8(str_value));
#ifdef DEBUG_MAFW
                qDebug() << "string: " << data.toString();
#endif
                emit static_cast<MafwRendererAdapter*>(user_data)->metadataChanged(QString(name), data);
            }
            break;

            case G_TYPE_INT:
            {
                int int_value = g_value_get_int(v);
                QVariant data = QVariant(int_value);
#ifdef DEBUG_MAFW
                qDebug() << "int: " << QString::number(data.toInt());
#endif
                emit static_cast<MafwRendererAdapter*>(user_data)->metadataChanged(QString(name), data);
            }
            break;

            case G_TYPE_INT64:
            {
                qint64 int64_value = g_value_get_int64(v);
                QVariant data = QVariant(int64_value);
                emit static_cast<MafwRendererAdapter*>(user_data)->metadataChanged(QString(name), data);
            }
            break;

            case G_TYPE_BOOLEAN:
            {
                bool bool_value = g_value_get_boolean(v);
                QVariant data = QVariant(bool_value);
                emit static_cast<MafwRendererAdapter*>(user_data)->metadataChanged(QString(name), data);
            }
            break;

            default:
            {
                qDebug() << "Unknown metadata type for" << name;
            }
            break;
        }

    }
}

void MafwRendererAdapter::onPlaylistChanged(MafwRenderer*,
                                            GObject* playlist,
                                            gpointer user_data)
{
#ifdef DEBUG_MAFW
    qDebug() << "On playlist changed";
#endif
    emit static_cast<MafwRendererAdapter*>(user_data)->playlistChanged(playlist);
}

void MafwRendererAdapter::onStateChanged(MafwRenderer*,
                                         gint state,
                                         gpointer user_data)
{
    MafwRendererAdapter *adapter = static_cast<MafwRendererAdapter*>(user_data);

#ifdef DEBUG_MAFW
    qDebug() << "On state changed";
#endif
    emit adapter->stateChanged(state);

#ifndef LIBPLAYBACK_FULL
    if (adapter->playback) {
        if (state == Paused || state == Stopped)
            pb_playback_req_state(static_cast<MafwRendererAdapter*>(user_data)->playback,
                                  PB_STATE_STOP, playback_state_req_callback, NULL);

        else if (state == Playing && !adapter->compatiblePlayback)
            pb_playback_req_state(static_cast<MafwRendererAdapter*>(user_data)->playback,
                                  PB_STATE_PLAY, playback_state_req_callback, NULL);
    }
#endif
}

void MafwRendererAdapter::play()
{
#ifdef MAFW_WORKAROUNDS
    // Early play() or gotoIndex() seems be reliable only for smaller libraries.
    // For bigger ones something probably doesn't have enough time to ready up.
    // Possible workaround is to call getSize() first, so when it returns,
    // we know that play() can be successfully called. That's only a theory,
    // but it works. ;)
    playlist->getSize();
#endif

#ifdef LIBPLAYBACK_FULL
    if (playback) {
        req_state_cb_payload *pl = new req_state_cb_payload;
        pl->adapter = this;
        pl->action = Play;
        pb_playback_req_state(playback, PB_STATE_PLAY,
                              playback_state_req_callback, pl);
    } else if (mafw_renderer) {
        mafw_renderer_play(mafw_renderer, MafwRendererSignalHelper::play_playback_cb, this);
    }
#else
    if (mafw_renderer)
        mafw_renderer_play(mafw_renderer, MafwRendererSignalHelper::play_playback_cb, this);
#endif
}

void MafwRendererAdapter::playObject(const gchar* object_id)
{
#ifdef LIBPLAYBACK_FULL
    if (mafw_renderer) {
        req_state_cb_payload *pl = new req_state_cb_payload;
        pl->adapter = this;
        pl->action = Dummy;
        pb_playback_req_state(playback, PB_STATE_PLAY,
                              playback_state_req_callback, pl);

        mafw_renderer_play_object(mafw_renderer, object_id, MafwRendererSignalHelper::play_object_playback_cb, this);
    }
#else
    if (mafw_renderer)
        mafw_renderer_play_object(mafw_renderer, object_id, MafwRendererSignalHelper::play_object_playback_cb, this);
#endif
}

void MafwRendererAdapter::playURI(const gchar* uri)
{
#ifdef LIBPLAYBACK_FULL
    if (mafw_renderer) {
        req_state_cb_payload *pl = new req_state_cb_payload;
        pl->adapter = this;
        pl->action = Dummy;
        pb_playback_req_state(playback, PB_STATE_PLAY,
                              playback_state_req_callback, pl);

        mafw_renderer_play_uri(mafw_renderer, uri, MafwRendererSignalHelper::play_uri_playback_cb, this);
    }
#else
    if (mafw_renderer)
        mafw_renderer_play_uri(mafw_renderer, uri, MafwRendererSignalHelper::play_uri_playback_cb, this);
#endif
}

void MafwRendererAdapter::stop()
{
#ifdef LIBPLAYBACK_FULL
    if (playback) {
        req_state_cb_payload *pl = new req_state_cb_payload;
        pl->adapter = this;
        pl->action = Stop;
        pb_playback_req_state(playback, PB_STATE_STOP,
                              playback_state_req_callback, pl);
    } else if (mafw_renderer) {
        mafw_renderer_stop(mafw_renderer, MafwRendererSignalHelper::stop_playback_cb, this);
    }
#else
    if (mafw_renderer)
        mafw_renderer_stop(mafw_renderer, MafwRendererSignalHelper::stop_playback_cb, this);
#endif
}

void MafwRendererAdapter::pause()
{
#ifdef LIBPLAYBACK_FULL
    if (playback) {
        req_state_cb_payload *pl = new req_state_cb_payload;
        pl->adapter = this;
        pl->action = Pause;
        pb_playback_req_state(playback, PB_STATE_STOP,
                              playback_state_req_callback, pl);
    } else if (mafw_renderer) {
        mafw_renderer_pause(mafw_renderer, MafwRendererSignalHelper::pause_playback_cb, this);
    }
#else
    if (mafw_renderer)
        mafw_renderer_pause(mafw_renderer, MafwRendererSignalHelper::pause_playback_cb, this);
#endif
}

void MafwRendererAdapter::resume()
{
#ifdef LIBPLAYBACK_FULL
    if (playback) {
        req_state_cb_payload *pl = new req_state_cb_payload;
        pl->adapter = this;
        pl->action = Resume;
        pb_playback_req_state(playback, PB_STATE_PLAY,
                              playback_state_req_callback, pl);
    } else if (mafw_renderer) {
        mafw_renderer_resume(mafw_renderer, MafwRendererSignalHelper::resume_playback_cb, this);
    }
#else
    if (mafw_renderer)
        mafw_renderer_resume(mafw_renderer, MafwRendererSignalHelper::resume_playback_cb, this);
#endif
}

void MafwRendererAdapter::getStatus()
{
    if(mafw_renderer)
    {
        mafw_renderer_get_status(mafw_renderer, MafwRendererSignalHelper::get_status_cb, this);
    }
}

void MafwRendererAdapter::next()
{
    if(mafw_renderer)
    {
        mafw_renderer_next(mafw_renderer, MafwRendererSignalHelper::next_playback_cb, this);
    }
}

void MafwRendererAdapter::previous()
{
    if(mafw_renderer)
    {
        mafw_renderer_previous(mafw_renderer, MafwRendererSignalHelper::previous_playback_cb, this);
    }
}

void MafwRendererAdapter::gotoIndex(uint index)
{
#ifdef MAFW_WORKAROUNDS
    // Explained in play()
    playlist->getSize();
#endif

    if(mafw_renderer)
    {
        mafw_renderer_goto_index(mafw_renderer, index, MafwRendererSignalHelper::goto_index_playback_cb, this);
    }
}

void MafwRendererAdapter::setPosition(MafwRendererSeekMode seekmode,
                                      int seconds)
{
    if(mafw_renderer)
    {
        mafw_renderer_set_position(mafw_renderer, seekmode, seconds, MafwRendererSignalHelper::set_position_cb, this);
    }
}

void MafwRendererAdapter::getPosition()
{
    if(mafw_renderer)
    {
        mafw_renderer_get_position(mafw_renderer, MafwRendererSignalHelper::get_position_cb, this);
    }
}

void MafwRendererAdapter::getCurrentMetadata()
{
    if(mafw_renderer)
    {
        mafw_renderer_get_current_metadata(mafw_renderer, MafwRendererSignalHelper::get_current_metadata_cb, this);
    }
}

bool MafwRendererAdapter::assignPlaylist(MafwPlaylist* playlist)
{
    if(mafw_renderer)
    {
        return mafw_renderer_assign_playlist(mafw_renderer, playlist, NULL);
    }
    return false;
}

bool MafwRendererAdapter::isRendererReady()
{
    if(mafw_renderer)
        return true;
    else
        return false;
}

void MafwRendererAdapter::setVolume(int volume)
{
    if(mafw_renderer)
    {
        g_value_set_uint (&GVolume, volume);
        mafw_extension_set_property(MAFW_EXTENSION(this->mafw_renderer), MAFW_PROPERTY_RENDERER_VOLUME, &GVolume);
    }
}

void MafwRendererAdapter::getVolume()
{
    if(mafw_renderer)
    {
#ifdef DEBUG_MAFW
        qDebug("MafwRendererAdapter::getVolume");
#endif
        mafw_extension_get_property (MAFW_EXTENSION(this->mafw_renderer), MAFW_PROPERTY_RENDERER_VOLUME,
                                     MafwRendererSignalHelper::get_property_cb, this);
    }
}

void MafwRendererAdapter::setWindowXid(uint Xid)
{
    if(mafw_renderer)
    {
        mafw_extension_set_property_uint(MAFW_EXTENSION(this->mafw_renderer), MAFW_PROPERTY_RENDERER_XID, Xid);
    }
}

void MafwRendererAdapter::setColorKey(int colorKey)
{
    if(mafw_renderer)
    {
        // MAFW API docs state that this is a read-only property
        // however, the stock Maemo 5 player is changing this to
        // a static number.
        mafw_extension_set_property_int (MAFW_EXTENSION(this->mafw_renderer), MAFW_PROPERTY_RENDERER_COLORKEY, colorKey);
    }
}

void MafwRendererAdapter::setErrorPolicy(uint errorPolicy)
{
    if (mafw_renderer)
        mafw_extension_set_property_uint(MAFW_EXTENSION(mafw_renderer), MAFW_PROPERTY_RENDERER_ERROR_POLICY, errorPolicy);
}
