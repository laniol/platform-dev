<?php
/**
 * @file
 * Contains ECEmbeddedImageSubContext.
 */

use \Drupal\DrupalExtension\Context\DrupalSubContextBase;

/**
 * Behat context for test setup related to the Embedded Image feature.
 */
class ECEmbeddedImageSubContext extends DrupalSubContextBase {

  /**
   * Indicates if the custom content type was set up.
   *
   * @var bool
   */
  private $contentTypeWasSetUp = FALSE;

  /**
   * Creates a test_media_gallery content type with a test_flickr_set field.
   *
   * @Given there is a media gallery content type with a field to embed images from flickr
   */
  public function createMediaGalleryContentType() {
    $media_gallery = [
      'type' => 'test_media_gallery',
      'name' => 'Media Gallery',
      'base' => 'node_content',
      'description' => 'For testing Flickr embedded images',
      'custom' => 1,
      'modified' => 1,
      'locked' => 0,
    ];

    $media_gallery = node_type_set_defaults($media_gallery);
    node_type_save($media_gallery);

    $flickr_set_field = [
      'field_name' => 'test_flickr_set',
      'type' => 'media',
    ];
    field_create_field($flickr_set_field);

    $media_gallery_flickr_set_field = [
      'field_name' => 'test_flickr_set',
      'entity_type' => 'node',
      'bundle' => 'test_media_gallery',
      'label' => 'Flickr set',
      'required' => TRUE,
      'settings' => [
        'file_extensions' => 'jpg jpeg gif png txt doc docx xls xlsx pdf ppt pptx pps ppsx odt ods odp mp3 mov mp4 m4a m4v mpeg avi ogg oga ogv weba webp webm',
        'entity_translation_sync' => FALSE,
      ],
      'active' => 1,
      'widget' => [
        'type' => 'media_generic',
        'module' => 'media',
        'settings' => [
          'allowed_schemes' => [
            'flickr' => 'flickr',
          ],
          'allowed_types' => [
            'image' => 'image',
            'video' => 'video',
          ],
          'browser_plugins' => [
            'media_internet' => 'media_internet',
          ],
        ],
      ],
    ];

    field_create_instance($media_gallery_flickr_set_field);

    $contributor_role = user_role_load_by_name('contributor');

    // Grant permissions for contributors.
    user_role_grant_permissions(
      $contributor_role->rid,
      [
        'create test_media_gallery content',
        'edit own test_media_gallery content',
        'access media browser',
      ]
    );

    $this->contentTypeWasSetUp = TRUE;
  }

  /**
   * Removes the test_media_gallery content type and related permissions.
   *
   * @AfterScenario
   */
  public function removeMediaGalleryContentType() {
    if (!$this->contentTypeWasSetUp) {
      return;
    }

    $this->contentTypeWasSetUp = FALSE;

    $instance = field_info_instance('node', 'test_flickr_set', 'test_media_gallery');

    if ($instance) {
      field_delete_instance($instance);
    }

    field_delete_field('test_flickr_set');
    field_purge_batch(10);

    if (node_type_get_type('test_media_gallery')) {
      node_type_delete('test_media_gallery');
    }

    $contributor_role = user_role_load_by_name('contributor');
    user_role_revoke_permissions(
      $contributor_role->rid,
      [
        'create test_media_gallery content',
        'edit own test_media_gallery content',
        'access media browser',
      ]
    );
  }

}
