# Content Moderation

Arbitrary moderation states and unpublished drafts for content.

## Concepts

Content Moderation adds arbitrary moderation states to Backdrop core's
"unpublished" and "published" content states, and affects the behavior of content
revisions when content are published. Moderation states are tracked per-revision;
rather than moderating content, Content Moderation moderates revisions.

### 1 Arbitrary publishing states

In Backdrop, content may be either unpublished or published. In typical
configurations, unpublished content is accessible only to the user who created
the content and to users with administrative privileges; published content is
visible to any visitor. For simple workflows, this allows authors and editors to
maintain drafts of content. However, when content needs to be seen by multiple
people before it is published--for example, when a site has an editorial or
moderation workflow--there are limited ways to keep track of content' status.
Content Moderation provides moderation states, so that unpublished content may
be reviewed and approved before it gets published.

### 2 Node revision behavior

Content Moderation affects the behavior of Backdrop’s content revisions. When
revisions are enabled for a particular content type, editing a content creates a new
revision. This lets users see how a content has changed over time and revert
unwanted or accidental edits. Content Moderation maintains this revision
behavior: any time a content is edited, a new version is created.

When there are multiple versions of a content--it has been edited multiple times,
and each round of editing has been saved in a revision--there is one "current"
revision. The current revision will always be the revision displayed in the content
editing form when a user goes to edit a piece of content.

In Backdrop core, publishing a content makes the current revision visible to site
visitors (in a typical configuration). Once a content is published, its current
revision is always the published version. Content Moderation changes this; it
allows you to use an older revision of a content as the published version, while
continuing to edit a newer draft.

@see content_moderation-core_revisions.png
@see content_moderation-wm_revisions.png

Internally, Content Moderation does this by managing the version of the content
stored in the {node} table. Backdrop core looks in this table for the "current
revision" of a content. Backdrop core equates the "current revision" of a content with
both the editable revision and, if the content is published, the published
revision. Content Moderation separates these two concepts; it stores the
published revision of a content in the {node} table, but uses the latest revision
in the `{node_revision}` table when the content is edited. Content Moderation's
treatment of revisions is identical to that of Backdrop core until content is
published.

### 3 Moderation states and revisions

Content Moderation maintains moderation states for revisions, rather than for
content. Since each revision may reflect a unique version of content, the state may
need to be revisited when a new revision is created. This also allows users to
track the moderation history of a particular revision, right up through the
point where it is published.

Revisions are a linear; revision history may not fork. This means that only the
latest revision may be edited or moderated.

### Installation

Install the module and enable it according to Backdrop standards.

If the site previously used Drupal 7 and Workbench Moderation module installation
will attempt to fetch the configuration from Workbench Moderation.

After installation, enable moderation on a content type by visiting its
configuration page:

    Admin > Structure > Content Types > [edit Article]

In the tab block at the bottom of the form, select the "Publishing options" tab.
In this tab under "Default Options", Content Moderation has added a checkbox,
"Enable moderation of revisions". To enable moderation on this content type, check
the boxes labeled "create new revision" (required) and "enable moderation of
revisions", and then save the content type.

## Requirements

Content Moderation does not depend on Views. However, Content Moderation
does have Views integration, and it provides two useful views ("My Drafts" and
"Needs Review").

## Configuration

Content Moderation's configuration section is located at:

    Admin > Configuration > Content Moderation > Content Moderation

This administration section provides tabs to configure states, transitions, and
to check whether your permissions are configured to enable full use of
moderation features.

### 1 Configuring states

Content Moderation provides three default moderation states: "Draft", "Needs
Review", and "Published". The Draft and Published states are required. You can
edit, add, and remove states at:

    Admin > Configuration > Content Moderation > Content Moderation > States

### 2 Configuring transitions

Content Moderation also provides transitions between these three states. You
can add and remove transitions at:

    Admin > Configuration > Content Moderation > Content Moderation > Transitions

### 3 Checking permissions

In order to use moderation effectively, users need a complex set of permissions.
If non-administrative users encounter access denied (403) errors or fail to see
notifications about moderation states, the "Check permissions" tab can help you
determine what permissions are missing. Visit:

    Admin > Configuration > Content Moderation > Content Moderation > Check Permissions

Select a Backdrop role, an intended moderation task, and the relevant content types,
and Content Moderation will give you a report of possible missing permissions.
Permissions configuration depends heavily on your configuration, so the report
may flag permissions as missing even when a particular role has enough access to
perform a particular moderation task.

#### 3.1 Recommended permissions

For reference, these are the permission sets recommended by the "Check
Permissions" tab:

    Author:
      Node:
        access content
        view own unpublished content
        view revisions
        create [content type] content
        edit own [content type] content
      Content Moderation:
        view moderation messages
        use content_moderation my drafts tab

    Editor:
      Node:
        access content
        view revisions
        revert revisions
        edit any [content type] content
      Content:
        view all unpublished content
      Content Moderation:
        view moderation messages
        view moderation history
        use content_moderation my drafts tab
        use content_moderation needs review tab

    Moderator:
      Node:
        access content
        view revisions
        edit any [content type] content
      Content:
        view all unpublished content
      Content Moderation:
        view moderation messages
        view moderation history
        use content_moderation needs review tab
    Publisher
      Node:
        access content
        view revisions
        revert revisions
        edit any [content type] content
      Content:
        view all unpublished content
      Content Moderation:
        view moderation messages
        view moderation history
        use content_moderation needs review tab
        unpublish live revision

## Using the module

Once the module is installed and moderation is enabled for one or more content
types, users with permission may:

* Use the "Moderate" content tab to view moderation history and navigate versions.

## Troubleshooting

* If users get access denied (403) errors when creating, editing, moderating, or
  reverting moderated content, the "Check Permissions" tab in Content
  Moderation's administration section can help diagnose what access is missing.
  See heading 3.3 in this README.
* If you're building Views of moderation records, keep in mind that for a single
  content, there will be multiple revisions, and for each revision, there may be
  multiple moderation records. This means it will be very easy to end up with a
  View that shows particular content or revisions more than once. Try adding the
  "Content Moderation: Current" filter, or using Views' "Use grouping" option
  (under the "Advanced settings" heading on the view editing page).

### 1 Database schema

Content Moderation uses one table to track content moderation node history.

* content_moderation_node_history
  Stores individual moderation records related to each content revision. Each
  record stores the nid and vid of a node, the original moderation state and the
  new moderation state, the uid of the user who did the moderation, and a
  timestamp.

### 2 Views integration

Content Moderation provides Views integration so that site builders may
include moderation information in node and node revision views.

* Filters, fields, sorts, and arguments are provided for moderation record data.
* A relationship is provided from moderation records to the user who made the
  moderation change.
* A "content type is moderated" filter is provided on for content to help in
  creating lists of only moderated content.

## Feature roadmap

* Allow configuration of 'Draft' and 'Published' states.

## License

This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.

## Maintainers

* [herbdool](https://github.com/herbdool)
* Seeking more maintainers.

## Credit

Ported to Backdrop by [herbdool](https://github.com/herbdool) from Workbench
Moderation. Originally based on content_moderation module by eugenmayer.

Drupal Maintainers:

* <https://www.drupal.org/u/agentrickard>
* <https://www.drupal.org/u/becw>
* <https://www.drupal.org/u/caroltron>
* <https://www.drupal.org/u/das-peter>
* <https://www.drupal.org/u/dave-reid>
* <https://www.drupal.org/u/larowlan>
* and more.
