#include "stdlib.h"

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

#include "moto-sler-material-node.h"
#include "moto-material-param-data.h"
#include "moto-messager.h"

/* Temporary GLSL shader sources. */

const GLcharARB *vs_source = "\
uniform vec3 lightPosition;\
uniform float Kd;\
uniform float Ks;\
varying float lightIntensity;\
\
void main()\
{\
    vec3 position       = vec3(gl_ModelViewMatrix * gl_Vertex);\
    vec3 viewVec        = normalize(-position);\
    \
    vec3 normal         = normalize(gl_NormalMatrix * gl_Normal);\
    /* normal              = faceforward(-normal, viewVec, normal); */\
    \
    vec3 lightVec       = normalize(lightPosition - position);\
    vec3 reflectVec     = reflect(-lightVec, normal);\
\
    float diffuse       = max(dot(lightVec, normal), 0.0);\
\
    float spec = max(dot(reflectVec, viewVec), 0.0);\
    spec = pow(spec, 16.0);\
\
    lightIntensity = gl_Color*Kd*diffuse + Ks*spec;\
\
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\
}";

const GLcharARB *fs_source = "\
varying float lightIntensity;\
void main()\
{\
    gl_FragColor = vec4(vec3(lightIntensity), 1.0);\
    /* gl_FragColor = vec4(vec3(0.7), 1.0); */\
}";

/* forward */

static void moto_sler_material_node_use(MotoMaterialNode *self);

/* class SlerMaterialNode */

static GObjectClass *sler_material_node_parent_class = NULL;

struct _MotoSlerMaterialNodePriv
{
    gboolean use_external;
    GString *external_sler_project_filename;

    GLhandleARB prog;
    GLhandleARB vs;
    GLhandleARB fs;
    gboolean shader_made;
};

static void
moto_sler_material_node_dispose(GObject *obj)
{
    MotoSlerMaterialNode *self = (MotoSlerMaterialNode *)obj;

    g_string_free(self->priv->external_sler_project_filename, TRUE);

    sler_material_node_parent_class->dispose(obj);
}

static void
moto_sler_material_node_finalize(GObject *obj)
{
    MotoSlerMaterialNode *self = (MotoSlerMaterialNode *)obj;

    g_slice_free(MotoSlerMaterialNodePriv, self->priv);

    sler_material_node_parent_class->finalize(obj);
}

static void
moto_sler_material_node_init(MotoSlerMaterialNode *self)
{
    self->priv = g_slice_new(MotoSlerMaterialNodePriv);

    // TODO: Temporary! When internal Sler editor is integrated 'use_external' default value will be FALSE.
    self->priv->use_external = TRUE;
    self->priv->external_sler_project_filename = g_string_new("");

    self->priv->shader_made = FALSE;
}

static void
moto_sler_material_node_class_init(MotoSlerMaterialNodeClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;
    MotoMaterialNodeClass *mnclass = (MotoMaterialNodeClass *)klass;

    sler_material_node_parent_class = g_type_class_peek_parent(klass);

    goclass->dispose    = moto_sler_material_node_dispose;
    goclass->finalize   = moto_sler_material_node_finalize;

    mnclass->use = moto_sler_material_node_use;
}

G_DEFINE_TYPE(MotoSlerMaterialNode, moto_sler_material_node, MOTO_TYPE_MATERIAL_NODE);

/* methods of class SlerMaterialNode */

static gpointer get_material(MotoParam *param)
{
    return moto_param_get_node(param);
}

MotoSlerMaterialNode *moto_sler_material_node_new(const gchar *name)
{
    MotoSlerMaterialNode *self = \
        (MotoSlerMaterialNode *)g_object_new(MOTO_TYPE_SLER_MATERIAL_NODE, NULL);
    MotoNode *node = (MotoNode *)self;

    moto_node_set_name((MotoNode *)self, name);

    MotoParamBlock *pb;
    MotoParamData *pdata;

    /* params */

    pb = moto_param_block_new("main", "Main", (MotoNode *)self);
    moto_node_add_param_block(node, pb);

    moto_param_new("material", "Material", MOTO_PARAM_MODE_OUT, pb,
            pdata = moto_material_param_data_new(NULL));
    moto_param_data_set_cbs(pdata, NULL, NULL, get_material, NULL);

    return self;
}

GLint get_uni_loc(GLhandleARB prog, const GLcharARB *name)
{
    GLint loc = glGetUniformLocationARB(prog, name);
    if(loc == -1)
    {
        GString *msg = g_string_new("Uniform variable with name \"");
        g_string_append(msg, name);
        g_string_append(msg, "\" is not exist");
        moto_error(msg->str);
        g_string_free(msg, TRUE);
    }
    return loc;
}

void print_log_info(GLhandleARB handle)
{
    int info_log_length = 0;
    int chars_written = 0;
    GLcharARB *info_log;

    glGetObjectParameterivARB(handle,
        GL_OBJECT_INFO_LOG_LENGTH_ARB, &info_log_length);

    if(info_log_length > 0)
    {
        info_log = (GLcharARB*)g_try_malloc(sizeof(GLcharARB)*info_log_length);
        if(info_log == NULL)
        {
            moto_error("Out of memory (InfoLog)");
            exit(1);
        }
        glGetInfoLogARB(handle, info_log_length, &chars_written, info_log);
        moto_info(info_log);
        g_free(info_log);
    }
    else
    {
        moto_info("No info log :(");
    }
}

static gboolean make_shader(MotoSlerMaterialNode *self)
{
    GLint compile_status, link_status;

    /* vertex shader */
    self->priv->vs = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    glShaderSourceARB(self->priv->vs, 1, & vs_source, NULL);
    glCompileShaderARB(self->priv->vs);
    glGetObjectParameterivARB(self->priv->vs,
            GL_OBJECT_COMPILE_STATUS_ARB, & compile_status);

    if( ! compile_status)
    {
        moto_error("Compilation of vertex shader failed");
        print_log_info(self->priv->vs);
        return FALSE;
    }

    /* fragment shader */
    self->priv->fs = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
    glShaderSourceARB(self->priv->fs, 1, & fs_source, NULL);
    glCompileShaderARB(self->priv->fs);
    glGetObjectParameterivARB(self->priv->fs,
            GL_OBJECT_COMPILE_STATUS_ARB, & compile_status);

    if( ! compile_status)
    {
        moto_error("Compilation of fragment shader failed");
        print_log_info(self->priv->fs);
        return FALSE;
    }

    /* program */
    self->priv->prog = glCreateProgramObjectARB();
    glAttachObjectARB(self->priv->prog, self->priv->vs);
    glAttachObjectARB(self->priv->prog, self->priv->fs);
    glLinkProgramARB(self->priv->prog);
    glGetObjectParameterivARB(self->priv->prog,
        GL_OBJECT_LINK_STATUS_ARB, & link_status);

    if( ! link_status)
    {
        moto_error("Linking failed");
        print_log_info(self->priv->prog);
        return FALSE;
    }

    return TRUE;
}

static void moto_sler_material_node_use(MotoMaterialNode *self)
{
    MotoSlerMaterialNode *slernode = (MotoSlerMaterialNode *)self;

    if( ! slernode->priv->shader_made)
        slernode->priv->shader_made = make_shader(slernode);

    if(slernode->priv->shader_made)
    {
        glUniform3fARB(get_uni_loc(slernode->priv->prog, "lightPosition"),
                0.3, 0.15, 0.15);
        glUniform1fARB(get_uni_loc(slernode->priv->prog, "Kd"), 1);
        glUniform1fARB(get_uni_loc(slernode->priv->prog, "Ks"), 0.3);
        glUseProgramObjectARB(slernode->priv->prog);
    }
}

/* class SlerMaterialNodeFactory */

MotoNode *
moto_sler_material_node_factory_create_node(MotoNodeFactory *self,
        const gchar *name);

static GType moto_sler_material_node_factory_get_node_type(MotoNodeFactory *self);

static GObjectClass *sler_material_node_factory_parent_class = NULL;

static void
moto_sler_material_node_factory_dispose(GObject *obj)
{
    sler_material_node_factory_parent_class->dispose(obj);
}

static void
moto_sler_material_node_factory_finalize(GObject *obj)
{
    sler_material_node_factory_parent_class->finalize(obj);
}

static void
moto_sler_material_node_factory_init(MotoSlerMaterialNodeFactory *self)
{

}

static void
moto_sler_material_node_factory_class_init(MotoSlerMaterialNodeFactoryClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;
    MotoNodeFactoryClass *nfclass = (MotoNodeFactoryClass *)klass;

    sler_material_node_factory_parent_class = g_type_class_peek_parent(klass);

    goclass->dispose    = moto_sler_material_node_factory_dispose;
    goclass->finalize   = moto_sler_material_node_factory_finalize;

    nfclass->get_node_type  = moto_sler_material_node_factory_get_node_type;
    nfclass->create_node    = moto_sler_material_node_factory_create_node;
}

G_DEFINE_TYPE(MotoSlerMaterialNodeFactory, moto_sler_material_node_factory, MOTO_TYPE_NODE_FACTORY);

/* methods of class SlerMaterialNodeFactory */

static MotoNodeFactory *sler_material_node_factory = NULL;

MotoNodeFactory *moto_sler_material_node_factory_new()
{
    if( ! sler_material_node_factory)
        sler_material_node_factory = \
            (MotoNodeFactory *)g_object_new(MOTO_TYPE_SLER_MATERIAL_NODE_FACTORY, NULL);

    return sler_material_node_factory;
}

MotoNode *moto_sler_material_node_factory_create_node(MotoNodeFactory *self,
        const gchar *name)
{
    return (MotoNode *)moto_sler_material_node_new(name);
}

static GType moto_sler_material_node_factory_get_node_type(MotoNodeFactory *self)
{
    return MOTO_TYPE_SLER_MATERIAL_NODE;
}
